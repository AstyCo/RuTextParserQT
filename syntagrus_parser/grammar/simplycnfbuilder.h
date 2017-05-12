#ifndef SIMPLYCNFBUILDER_H
#define SIMPLYCNFBUILDER_H

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

class SYNTAGRUS_PARSERSHARED_EXPORT SimplyCNFBuilder
{
    typedef QPair<RuleDirection, nonterminal> DirectedNonterminal;
    typedef QList<DirectedNonterminal> RuleBody;
    typedef QList<RuleBody> Rules;

    FirstCFG *_cfg;
    FirstCNF _cnf;

public:
    SimplyCNFBuilder(FirstCFG *cfg);

    FirstCNF &cnf() { return _cnf;}
    const FirstCNF &cnf() const { return _cnf;}

    void build();

    void produceRules(nonterminal srcNT, const Rules &rules);
    void produceRootRules(const Rules &rules);
};

#endif // SIMPLYCNFBUILDER_H
