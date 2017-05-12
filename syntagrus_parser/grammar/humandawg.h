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
#include <QSharedPointer>

typedef QPair<RuleDirection, nonterminal> LabelType;
struct NodeDAWG
{
    QMap<LabelType, QSharedPointer<NodeDAWG> > childs;
    bool final;
    NodeDAWG() : final(false) {}

    int finalCount() const;
    int intermediateCount() const;

    bool isEmpty() const { return childs.isEmpty();}


    // algorithm things
    void prepare();
    void prepareThis();

    bool visited;
    nonterminal nt;
    QMap<nonterminal, nonterminal> len2fix;
};
using namespace dawgdic;

class HumanDAWG
{
    typedef QMap<BaseType, QSet<QByteArray> > IndexPaths;

    QSharedPointer<NodeDAWG> _root;

    QMap<BaseType, QSharedPointer<NodeDAWG> > _hashIndexToNode;

public:
    HumanDAWG(Completer *completer);

    bool contains(const QList<LabelType> &body) const;
    const QSharedPointer<NodeDAWG> &root() const { return _root;}
    QSharedPointer<NodeDAWG> &root() { return _root;}

    int finalCount() const;
    int intermediateCount() const;

private:
    void buildFromCompleter(Completer *completer);
    IndexPaths makeStep4(BaseType index);
    IndexPaths makeStep41(BaseType index);
    IndexPaths makeStep4n(const IndexPaths &lastPaths, int n);
    void makeStep4H(BaseType index, QSharedPointer<NodeDAWG> node);

    LabelType ltFromByteArray(const QByteArray &s);

    const Dictionary &_dic;
    const Guide &_guide;
};

#endif // HUMANDAWG_H
