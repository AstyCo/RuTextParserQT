#ifndef HUMANDAWG_H
#define HUMANDAWG_H

#include "dawgdic/completer.h"
#include "syntagrus_parser_global.h"
#include "firstcfg.h"
#include "rutextparser_extensions/dumpable.h"
#include "internal/base-types.h"
#include "corpora_parsing/treecorpora.h"

#include "dawgdic/dawg-builder.h"
#include "dawgdic/dictionary-builder.h"


#include <QMap>
#include <QList>

typedef QPair<RuleDirection, nonterminal> LabelType;
struct NodeDAWG
{
    QMap<LabelType, NodeDAWG> childs;
    bool final;
    NodeDAWG() : final(false) {}

    int finalCount() const;
    int intermediateCount() const;

    bool isEmpty() const { return childs.isEmpty();}

    bool visited;
    nonterminal nt;
    void prepare();
    void prepareThis() { visited = false; nt = -1;}
};
using namespace dawgdic;

class HumanDAWG
{
    typedef QMap<BaseType, QSet<QByteArray> > IndexPaths;

    NodeDAWG _root;

public:
    HumanDAWG(Completer *completer);

    bool contains(const QList<LabelType> &body) const;
    const NodeDAWG &root() const { return _root;}
    NodeDAWG &root() { return _root;}

    int finalCount() const;
    int intermediateCount() const;

private:
    void buildFromCompleter(Completer *completer);
    IndexPaths makeStep4(BaseType index);
    IndexPaths makeStep41(BaseType index);
    IndexPaths makeStep4n(const IndexPaths &lastPaths, int n);
    void makeStep4H(BaseType index, NodeDAWG &node);

    LabelType ltFromByteArray(const QByteArray &s);

    const Dictionary &_dic;
    const Guide &_guide;
};

#endif // HUMANDAWG_H
