#ifndef FIRSTTOCNFDAWG_H
#define FIRSTTOCNFDAWG_H

#include "syntagrus_parser_global.h"
#include "firstcfg.h"
#include "rutextparser_extensions/dumpable.h"
#include "internal/base-types.h"
#include "corpora_parsing/treecorpora.h"
#include "grammar/humandawg.h"
#include "firstcnf.h"

#include "dawgdic/dawg-builder.h"
#include "dawgdic/dictionary-builder.h"


#include <QMap>
#include <QList>

class SYNTAGRUS_PARSERSHARED_EXPORT FirstToCNFDAWG
{
    typedef QPair<RuleDirection, nonterminal> DirectedNonterminal;
    typedef QList<DirectedNonterminal> RuleBody;
    typedef QList<RuleBody> Rules;

    FirstCFG *_cfg;
    FirstCNF _cnf;

public:
    FirstToCNFDAWG(FirstCFG *cfg);

    FirstCNF &cnf() { return _cnf;}
    const FirstCNF &cnf() const { return _cnf;}


    void buildFirstCNF();
    void produceRules(nonterminal fidSrc, HumanDAWG &dawg);
    void produceRules(nonterminal fidSrc, NodeDAWG &node);


    void testHDAWG(const HumanDAWG &dawg, const Rules& rules);

private:
    void replaceOrDuplicateRules();

    QMap<nonterminal, QSet<nonterminal> > _toRepOrDup;

};

#endif // FIRSTTOCNFDAWG_H
