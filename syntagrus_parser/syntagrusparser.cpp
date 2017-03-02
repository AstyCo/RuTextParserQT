#include "syntagrusparser.h"

#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QPair>

SynTagRusParser::SynTagRusParser()
    :_grammar(new CNFGrammar())
{
}

SynTagRusParser::~SynTagRusParser()
{
    delete _grammar;
}

void SynTagRusParser::parseAll()
{
    QDir dir(_synTagRusDirectory);
    QStringList files = dir.entryList(QStringList("*.tgt"));

    foreach(const QString &fileName, files)
        parseXml(fileName);
}

void SynTagRusParser::parseXml(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("Can not open SynTagRus.xml");
        return false;
    }
    QDomDocument doc();

    if (!doc.setContent(&file)) {
        qWarning(QString("error on reading %1").arg(path).toLocal8Bit().data());
        file.close();
        return;
    }
    file.close();

    foreach (QDomNode bodyNode, doc.elementsByTagName("body")){
        QDomElement bodyElem = bodyNode.toElement();
        if (bodyElem.isNull()) {
            qWarning("bodyNode not an Element");
            continue;
        }

        QDomNodeList sequenceNodes = bodyElem.elementsByTagName("S");
        foreach (QDomNode sequenceNode, sequenceNodes){
            QDomElement sequenceElem = sequenceNode.toElement();
            if (sequenceElem.isNull()) {
                qWarning("sequenceNode not an Element");
                continue;
            }

            parseSequence(sequenceElem);
        }
    }


    QDomNode n = docElem.firstChild();
    while (!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull()) {
            cout << qPrintable(e.tagName()) << endl; // the node really is an element.
        }
        n = n.nextSibling();
    }
}

void SynTagRusParser::parseSequence(const QDomElement &sequenceElement)
{
    QMap<int, QStringList> rules;
    QMap<int, QString> idToFeature;

    QDomElement wordElement = sequenceElement.firstChildElement("W");
    while (!wordElement.isNull()) {
        QDomAttr domAttr = wordElement.attributeNode("DOM");
        int dom;

        // get id
        if (domAttr.value() == "_root")
            dom = -1;
        else
            dom = domAttr.value().toInt();

        if(dom == 0){
            qWarning(QString("DOM attribute conversion error %1").arg(domAttr.value()).toLocal8Bit().data());
            continue;
        }

        int id = wordElement.attributeNode("ID").value().toInt();
        if(id == 0){
            qWarning(QString("ID attribute conversion error %1").arg(wordElement.text()).toLocal8Bit().data());
            continue;
        }

        QString feature = wordElement.attributeNode("FEAT").value();
        if(feature.isEmpty()){
            qWarning(QString("FEAT attribute error %1").arg(wordElement.text()).toLocal8Bit().data());
            continue;
        }

        QString link = wordElement.attributeNode("LINK").value();
//        if(link.isEmpty()){
//            qWarning(QString("LINK attribute error %1").arg(wordElement.text()).toLocal8Bit().data());
//            continue;
//        }

        idToFeature.insert(id, feature);

        // make rules
        if (dom == -1)  // _root
            rules.insert(dom, QStringList(feature));
        else
            insertRule(&rules, dom, link, feature);


        // make step
        wordElement = wordElement.nextSiblingElement("W");
    }

    // leading in CNF
    QMap<int, QStringList>::iterator i(rules.begin());
    while (i != rules.end()) {
        if (i.value().isEmpty()) {
            qCritical("Leading in CNF :: rule with Empty right side");
            return;
        }

        addCNFRules(i, idToFeature);
    }
}

void SynTagRusParser::insertRule(QMap<int, QStringList> *rules, int dom, QString link, QString feature)
{
    QStringList sl;
    sl << link << feature;  // order is important

    if (rules->contains(dom))
        (*rules)[dom].append(sl);
    else
        rules->insert(dom, sl);
}

void SynTagRusParser::addCNFRules(QMap<int, QStringList>::Iterator &i, const QMap<int, QString> &idToFeature)
{
    if (i.key() == -1) {
       _grammar->addRule(RuleCNFGrammar("_root", i.value().first()));
    }
    else if (i.value().size() < 2) {
        qWarning("SynTagRusParser::addCNFRules");
        return;
    }
    else {
        QString lastGeneratedName = generateUniqueName();

        _grammar->addRule(RuleCNFGrammar(idToFeature[i.key()], idToFeature[i.key()], lastGeneratedName));

        for (int j = 0; j < i.value().size(); ++j) {
            _grammar->addRule(RuleCNFGrammar(lastGeneratedName, lastGeneratedName, i.value()[j]));
        }
    }
}

QString SynTagRusParser::generateUniqueName() const
{
    static long long n = 1;

    return "S" + QString::number(n++);
}
