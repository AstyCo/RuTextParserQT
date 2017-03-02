#ifndef SYNTAGRUSPARSER_H
#define SYNTAGRUSPARSER_H

#include "syntagrus_parser_global.h"
#include "cnfgrammar.h"

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
};

#endif // SYNTAGRUSPARSER_H
