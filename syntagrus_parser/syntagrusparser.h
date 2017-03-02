#ifndef SYNTAGRUSPARSER_H
#define SYNTAGRUSPARSER_H

#include "syntagrus_parser_global.h"
#include "cnfgrammar.h"

#include <QMap>
#include <QString>
#include <QDomDocument>

class SYNTAGRUS_PARSERSHARED_EXPORT SynTagRusParser
{
    static QString _synTagRusDirectory = "syntagrus04.07.2012";
    CNFGrammar *_grammar;
public:
    SynTagRusParser();
    ~SynTagRusParser();

    void parseAll();
    void parseXml(const QString &path);
    void parseSequence(const QDomElement &sequenceElement);
    static void insertRule(QMap<int, QStringList> *rules, int dom, QString link, QString feature);
    static void addCNFRules(QMap<int, QStringList>::Iterator &i, const QMap<int, QString> &idToFeature);
    QString generateUniqueName() const;
};

#endif // SYNTAGRUSPARSER_H
