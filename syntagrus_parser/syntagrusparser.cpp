#include "syntagrusparser.h"

#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QPair>
#include <QDirIterator>

#include <QDebug>

SynTagRusParser::SynTagRusParser()
    :_grammar(new CNFGrammar()), _synTagRusDirectory("../../RuTextParserQT/syntagrus04.07.2012")
{
}

SynTagRusParser::~SynTagRusParser()
{
    delete _grammar;
}

void SynTagRusParser::parse(const QString &path)
{
    QString synTagRusPath =  ( path.isEmpty()
                                 ? _synTagRusDirectory
                                 : path );

    QDirIterator it(synTagRusPath, QStringList() << "*.tgt", QDir::Files, QDirIterator::Subdirectories);
//    int kk = 0;
    while (it.hasNext()) {
//        if (kk++ > 5)
//            return;

        parseXml(it.next());
        qDebug() << it.filePath() << "PARSED";
    }
    qDebug() << "PARSING FINISHED";
}

CNFGrammar *SynTagRusParser::getGrammar()
{
    return _grammar;
}

void SynTagRusParser::parseXml(const QString &path)
{
    QFile file(path);
    if (!file.exists()) {
        qCritical(QString("File %1 doesn't exist").arg(path).toLocal8Bit().data());
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical(QString("Can not open %1").arg(path).toLocal8Bit().data());
        return;
    }
    QDomDocument doc;

    if (!doc.setContent(&file)) {
        qWarning(QString("error on reading %1").arg(path).toLocal8Bit().data());
        file.close();
        return;
    }
    file.close();

    QDomElement bodyElem = doc.documentElement().firstChildElement("body");
    while (!bodyElem.isNull()){
        QDomElement sequenceElem =  bodyElem.firstChildElement("S");
        while (!sequenceElem.isNull()) {
            parseSequence(sequenceElem, path + ':' + sequenceElem.attribute("ID"));

            sequenceElem = sequenceElem.nextSiblingElement("S");
        }

        bodyElem = bodyElem.nextSiblingElement("body");    // go for all <body>
    }
}

void SynTagRusParser::parseSequence(const QDomElement &sequenceElement, const QString &tmp)
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

        if (dom == 0){
            qWarning(QString("DOM attribute conversion error %1").arg(domAttr.value()).toLocal8Bit().data());

            wordElement = wordElement.nextSiblingElement("W");
            continue;
        }

        int id = wordElement.attributeNode("ID").value().toInt();
        if (id == 0){
            qWarning(QString("ID attribute conversion error %1").arg(wordElement.text()).toLocal8Bit().data());

            wordElement = wordElement.nextSiblingElement("W");
            continue;
        }

        QString feature = wordElement.attributeNode("FEAT").value();
        if (feature.isEmpty()){
            if (wordElement.attribute("NODETYPE") != "FANTOM")
                qWarning(QString("FEAT attribute error %1").arg(tmp).toLocal8Bit().data());

            wordElement = wordElement.nextSiblingElement("W");
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

        i++;
    }
}

void SynTagRusParser::insertRule(QMap<int, QStringList> *rules, int dom, QString link, QString feature)
{
    QStringList sl;
    sl << link << feature;  // order is important

    if (rules->contains(dom)) {
        (*rules)[dom].append(sl);
//        qDebug() << QString("rules[%1].append(%2)").arg(QString::number(dom)).arg(sl.join(" "));
    }
    else {
        rules->insert(dom, sl);
//        qDebug() << "rules insert" << dom << sl;
    }

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
        generationMethod1(i, idToFeature);
    }
}

QString SynTagRusParser::generateUniqueName() const
{
    static long long n = 1;

    return "S" + QString::number(n++);
}

void SynTagRusParser::generationMethod1(QMap<int, QStringList>::Iterator &i, const QMap<int, QString> &idToFeature)
{
    QString lastGeneratedName = generateUniqueName();

    // creating first rule
    _grammar->addRule(RuleCNFGrammar(idToFeature[i.key()], idToFeature[i.key()], lastGeneratedName));

    // creating most rules
    for (int j = 0; j < i.value().size() - 1; ++j) {
        QString newGeneratedName = generateUniqueName();
        _grammar->addRule(RuleCNFGrammar(lastGeneratedName, i.value()[j], newGeneratedName));
        lastGeneratedName = newGeneratedName;
    }

    // creating last rule


}
