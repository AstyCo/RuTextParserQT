#ifndef FIRSTCFG_H
#define FIRSTCFG_H

#include "rutextparser_extensions/dumpable.h"
#include "internal/base-types.h"
#include "corpora_parsing/treecorpora.h"
#include "ruledirection.h"

#include <QMap>
#include <QList>
#include <QDataStream>

class FirstCFG: public Dumpable
{
    typedef QPair<RuleDirection, nonterminal> DirectedNonterminal;
    typedef QList<DirectedNonterminal> RuleBody;
    typedef QList<RuleBody> Rules;
    typedef QPair<RuleDirection, QPair<linkID, featureID> > TDep;

    typedef UniqueVector<nonterminal, featureID> TerminalMapper;



    TerminalMapper _terminalRules;
    QMap<nonterminal, Rules> _nonterminalRules;
    QMap<nonterminal, QList<QSet<QList<linkID> > > > _linksForRules;

//    nonterminal _nextNonterminal;
public:
    FirstCFG(QString dumpName) : Dumpable(dumpName)/*, _nextNonterminal(20000)*/
    {
        initRules();
    }
    // getters
    const TerminalMapper &terminalRules() const { return _terminalRules;}
    const QMap<nonterminal, Rules> &nonterminalRules() const { return _nonterminalRules;}
    const QMap<nonterminal, QList<QSet<QList<linkID> > > > & linksForRules() const { return _linksForRules;}


    void addRoot(const QList<TDep> &deps)
    {
        addToSrc(rootNT(), deps);
    }

    void add(const featureID &src, const QList<TDep> &deps)
    {
        nonterminal srcNT;
        if (!_terminalRules.contains(src))
            _terminalRules.append(src);

        srcNT = _terminalRules.index(src);

        addToSrc(srcNT, deps);
    }

    void insertRuleBody(nonterminal src, const RuleBody &rb, const QList<linkID> &links)
    {
        if (!_nonterminalRules.contains(src))
            _nonterminalRules.insert(src, Rules());

        int ruleBodyIndex = _nonterminalRules[src].indexOf(rb);

        if (ruleBodyIndex == -1) {
            // not found
            _nonterminalRules[src].append(rb);

            ruleBodyIndex = _nonterminalRules[src].indexOf(rb);
            _linksForRules[src].append(QSet<QList<linkID> >());
            _linksForRules[src][ruleBodyIndex].insert(links);
        }
        else {
            // found
            _linksForRules[src][ruleBodyIndex].insert(links);
        }
    }

    nonterminal rootNT() const { return 1;}
//    nonterminal generateNewNonterminal() { return _nextNonterminal++;}

    void clear()
    {
        _terminalRules.clear();
        _nonterminalRules.clear();
//        _nextNonterminal = 20000;
        initRules();
    }
    // info
    long ruleCountTotal() const {
        return rootRuleCount() + nonrootRuleCount();
    }

    long nonrootRuleCount() const {
        long result = 0;
        QMap<nonterminal, Rules>::const_iterator it(_nonterminalRules.begin());
        while (it != _nonterminalRules.constEnd()) {
            if (it.key() != rootNT()) {
                result += it.value().size();
            }

            ++it;
        }
        return result;
    }
    long rootRuleCount() const {
        return _nonterminalRules[rootNT()].size();
    }

    long terminalRuleCount() const {
        return _terminalRules.size();
    }

    QString toReport() const {
        QString report("terminalRuleCount: %1\n"
                       "nonrootRuleCount: %2\n"
                       "rootRuleCount: %3\n"
                       "ruleCountTotal: %4\n");
        return report.arg(terminalRuleCount())
                .arg(nonrootRuleCount())
                .arg(rootRuleCount())
                .arg(ruleCountTotal());
    }

    EXT_DECLARE_SERIALIZATION

private:
    void initRules()
    {
        _nonterminalRules.insert(rootNT(), Rules());
        _terminalRules.append(-2); // occupies size for invalid, and rootNT
        _terminalRules.append(-1); // occupies size for invalid, and rootNT
    }

    void addToSrc(nonterminal srcNT, const QList<TDep> &deps)
    {
        RuleBody newRuleBody;
        QList<linkID> links;

        for (int i=0; i < deps.size(); ++i) {
            nonterminal depNT;
            if (!_terminalRules.contains(deps.at(i).second.second))
                _terminalRules.append(deps.at(i).second.second);

            depNT = _terminalRules.index(deps.at(i).second.second);

            newRuleBody.append(qMakePair(deps.at(i).first, depNT));
            links.append(deps.at(i).second.first);
        }

        insertRuleBody(srcNT, newRuleBody, links);
    }


    friend QDataStream &operator<<(QDataStream &ds, const FirstCFG &s);
    friend QDataStream &operator>>(QDataStream &ds, FirstCFG &s);
};

inline QDataStream &operator<<(QDataStream &ds, const FirstCFG &s) {
    ds << s._terminalRules;
    ds << s._nonterminalRules;
    ds << s._linksForRules;
//    ds << s._nextNonterminal;

    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, FirstCFG &s) {
    ds >> s._terminalRules;
    ds >> s._nonterminalRules;
    ds >> s._linksForRules;
//    ds >> s._nextNonterminal;

    return ds;
}


#endif // FIRSTCFG_H
