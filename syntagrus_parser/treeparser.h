#ifndef TREEPARSER_H
#define TREEPARSER_H

#include "syntagrus_parser_global.h"

#include "corpora_parsing/treecorpora.h"
#include "grammar/cnfgrammar.h"

class SYNTAGRUS_PARSERSHARED_EXPORT TreeParser
{
    QString _dumpFilenameGrammar;
    CNFGrammar _grammar;
public:
    TreeParser();

    void parseTree(const TreeCorpora &treeCorpora);

    CNFGrammar *getGrammar();
    const CNFGrammar *getGrammar() const;

    void serializeGrammar() const;
    void deserializeGrammar();

private:
    void parseSentence(const SentenceInCorpora &sentence);
    void parseNode(const QString &leftNonterminal, const RecordNode *node);
    void produceTerminalRule(const QString &leftNonterminal, const RecordNode *node);
    void produceNonterminalRules(QString leftNonterminal, QList<RecordNode *> &nodes);
    void produceDotNonterminalRule(const QString &leftNonterminal, const RecordNode *node,
                                   QList<RecordNode *> &nodes, bool dotThenRule = true);

    QString generateUniqueNonterminalName() const;
};

#endif // TREEPARSER_H
