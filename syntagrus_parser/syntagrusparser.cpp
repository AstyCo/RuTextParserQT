#include "syntagrusparser.h"

#include <QFile>
#include <QDir>
#include <QDomDocument>

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

    foreach(QDomNode bodyNode, doc.elementsByTagName("body")){
        QDomElement bodyElem = bodyNode.toElement();
        if (bodyElem.isNull()) {
            qWarning("bodyNode not an Element");
            continue;
        }

        QDomNodeList sequenceNodes = bodyElem.elementsByTagName("S");
        foreach(QDomNode sequenceNode, sequenceNodes){
            QDomElement sequenceElem = sequenceNode.toElement();
            if (sequenceElem.isNull()) {
                qWarning("sequenceNode not an Element");
                continue;
            }

            parseSequence(sequenceElem);
        }
    }


    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            cout << qPrintable(e.tagName()) << endl; // the node really is an element.
        }
        n = n.nextSibling();
    }
}

void SynTagRusParser::parseSequence(const QDomElement &sequenceElement)
{
    QDomElement wordElement = sequenceElement.firstChildElement("W");
    while(!wordElement.isNull()){
        /// TODO

    }
}
