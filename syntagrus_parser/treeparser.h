#ifndef TREEPARSER_H
#define TREEPARSER_H

#include "syntagrus_parser_global.h"

#include "corpora_parsing/optimizedtreecorpora.h"
#include "grammar/cnfgrammar.h"

class SYNTAGRUS_PARSERSHARED_EXPORT TreeParser
{
    QString _dumpFilenameGrammar;

    CNFGrammar _grammar;
public:
    TreeParser();

    void parseTree(const OptimizedTreeCorpora &treeCorpora);
    void parseTree(const TreeCorpora &treeCorpora);


    CNFGrammar *getGrammar();
    const CNFGrammar *getGrammar() const;

    void serializeGrammar() const;
    void deserializeGrammar();

private:
    void parseSentence(const OptimizedSentence &sentence);
    void parseByDom(const OptimizedSentence &sentence,
                    const Nonterminal &leftNonterminal,
                    const WordIndex &dom);
    void parseSentence(const SentenceInCorpora &sentence);
    void parseNode(const Nonterminal &leftNonterminal, const RecordNode *node);
    void produceTerminalRule(const Nonterminal &leftNonterminal, const RecordNode *node);
    void produceNonterminalRules(Nonterminal leftNonterminal, QList<RecordNode *> &nodes);
    void produceDotNonterminalRule(const Nonterminal &leftNonterminal, const RecordNode *node,
                                   QList<RecordNode *> &nodes, bool dotThenRule = true);

    Nonterminal generateUniqueNonterminalName() const;
};

#endif // TREEPARSER_H
