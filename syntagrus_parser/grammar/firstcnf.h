#ifndef FIRSTCNF_H
#define FIRSTCNF_H

#include "rutextparser_extensions/dumpable.h"
#include "internal/base-types.h"
#include "corpora_parsing/treecorpora.h"
#include "ruledirection.h"

#include <QMap>
#include <QSet>
#include <QList>
#include <QDataStream>

class SYNTAGRUS_PARSERSHARED_EXPORT FirstCNF : public Dumpable
{
    typedef UniqueVector<nonterminal, featureID> TerminalMapper;
    typedef QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > > BinaryRules;
    TerminalMapper _terminalRules;
    QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > > _binaryRules;

    QMap<nonterminal, QSet<nonterminal> > _hashRuleAppearance;

    nonterminal nextFreeNT;

public:
    FirstCNF();

    const TerminalMapper &terminalRules() const { return _terminalRules;}
    const QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > > &binaryRules() const { return _binaryRules;}

    void setNextFreeNT(nonterminal next) { nextFreeNT = next;}
    nonterminal nextNT() { return nextFreeNT++;}
    void setTerminalRules(const TerminalMapper &tmapper) {
        for (nonterminal nt = 0; nt < tmapper.size(); ++nt)
            _terminalRules.append(tmapper.value(nt));
    }
    void add(nonterminal src, nonterminal b1, nonterminal b2);

    int terminalCount() const { return _terminalRules.size();}
    int binaryRulesCount() const;
    int nonterminalCount() const { return _binaryRules.size() + terminalCount();}

    QString toReport() const;
    QString toReportRules() const;

    void minimize();
    void recursiveMinimize();

    typedef QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > >::iterator BinaryRuleIterator;
    BinaryRuleIterator replace(BinaryRuleIterator &it2, BinaryRuleIterator &it);


    //    void duplicateOrReplace(nonterminal src, nonterminal dest);
    void replace(nonterminal src, nonterminal dest);
    //    void duplicate(nonterminal src, nonterminal dest);

    EXT_DECLARE_SERIALIZATION

    private:
        void fillAppearance();

    friend QDataStream &operator<<(QDataStream &ds, const FirstCNF &s);
    friend QDataStream &operator>>(QDataStream &ds, FirstCNF &s);
};


inline QDataStream &operator<<(QDataStream &ds, const FirstCNF &cnf)
{
    ds << cnf._terminalRules;
    ds << cnf._binaryRules;
    ds << cnf.nextFreeNT;

    return ds;
}
inline QDataStream &operator>>(QDataStream &ds, FirstCNF &cnf)
{
    ds >> cnf._terminalRules;
    ds >> cnf._binaryRules;
    ds >> cnf.nextFreeNT;

    return ds;
}

#endif // FIRSTCNF_H
