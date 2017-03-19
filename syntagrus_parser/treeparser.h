#ifndef TREEPARSER_H
#define TREEPARSER_H

#include "syntagrus_parser_global.h"
#include "corpora_parsing/treecorpora.h"
#include "grammar/cnfgrammar.h"

class SYNTAGRUS_PARSERSHARED_EXPORT TreeParser
{
    QString _dumpFilenameGrammar;

    CNFGrammar *_grammar;
public:
    TreeParser();

    void parseTree(const TreeCorpora &treeCorpora);

    CNFGrammar *getGrammar();
    const CNFGrammar *getGrammar() const;

    void serializeGrammar() const;
    void deserializeGrammar();

private:
    void parseSentence(const SentenceInCorpora &sentence,
                       const UniqueVector<featureID, QString> &fmapper, const UniqueVector<linkID, QString> &lmapper);
    void parseNode(const RecordNode *node,
                   const UniqueVector<featureID, QString> &fmapper, const UniqueVector<linkID, QString> &lmapper);
};

#endif // TREEPARSER_H
