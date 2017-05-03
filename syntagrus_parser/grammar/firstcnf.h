#ifndef FIRSTCNF_H
#define FIRSTCNF_H

#include "rutextparser_extensions/dumpable.h"
#include "internal/base-types.h"
#include "corpora_parsing/treecorpora.h"
#include "ruledirection.h"

#include <QMap>
#include <QList>
#include <QDataStream>

class SYNTAGRUS_PARSERSHARED_EXPORT FirstCNF
{
    QMap<nonterminal, featureID> _terminalRules;
    QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > > _binaryRules;

    nonterminal nextFreeNT;

public:
    FirstCNF();

    const QMap<nonterminal, featureID> &terminalRules() const { return _terminalRules;}
    const QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > > &binaryRules() const { return _binaryRules;}

    void setNextFreeNT(nonterminal next) { nextFreeNT = next;}
    nonterminal nextNT() { return nextFreeNT++;}
    void add(nonterminal nt, featureID t);
    void add(nonterminal src, nonterminal b1, nonterminal b2);

    int terminalCount() const { return _terminalRules.size();}
    int binaryRulesCount() const;
    int nonterminalCount() const { return _binaryRules.size() + terminalCount();}

    QString toReport() const;

    void minimize();

    void duplicateOrReplace(nonterminal src, nonterminal dest);
    void replace(nonterminal src, nonterminal dest);
    void duplicate(nonterminal src, nonterminal dest);

};



#endif // FIRSTCNF_H
