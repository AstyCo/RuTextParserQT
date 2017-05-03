#ifndef SECONDCFG_H
#define SECONDCFG_H

#include "rutextparser_extensions/dumpable.h"
#include "internal/base-types.h"
#include "corpora_parsing/treecorpora.h"
#include "ruledirection.h"

#include <QMap>
#include <QList>
#include <QDataStream>

class SecondCFG: public Dumpable
{
    typedef QPair<RuleDirection, nonterminal> DirectedNonterminal;
    typedef QList<DirectedNonterminal> RuleBody;
    typedef QList<RuleBody> Rules;
    typedef QPair<RuleDirection, QPair<linkID, featureID> > TDep;

    typedef UniqueVector<nonterminal, QPair<linkID, featureID> > TerminalMapper;

    TerminalMapper _terminalRules;
    QMap<nonterminal, Rules> _nonterminalRules;

//    nonterminal _nextNonterminal;
public:
    SecondCFG(QString dumpName) : Dumpable(dumpName)/*, _nextNonterminal(20000)*/
    {
        initRules();
    }
    // getters
    const TerminalMapper &terminalRules() const { return _terminalRules;}
    const QMap<nonterminal, Rules> &nonterminalRules() const { return _nonterminalRules;}

    void addRoot(const QList<TDep> &deps)
    {
        addToSrc(rootNT(), deps);
    }

    void add(const QPair<linkID, featureID> &src, const QList<TDep> &deps)
    {
        nonterminal srcNT;
        if (!_terminalRules.contains(src))
            _terminalRules.append(src);

        srcNT = _terminalRules.index(src);

        addToSrc(srcNT, deps);
    }

    void insertRuleBody(nonterminal src, const RuleBody &rb)
    {
        if (!_nonterminalRules.contains(src))
            _nonterminalRules.insert(src, Rules());

        int ruleBodyIndex = _nonterminalRules[src].indexOf(rb);

        if (ruleBodyIndex == -1) {
            // not found
            _nonterminalRules[src].append(rb);
        }
//        else {
//            // found
//        }
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

    void checkSameRules() const {
//        static int kk = 0;
        QList<QPair<nonterminal, nonterminal> > sameNT;
        QMap<nonterminal, Rules>::const_iterator it(_nonterminalRules.constBegin());

        while(it != _nonterminalRules.constEnd()) {
            QMap<nonterminal, Rules>::const_iterator it2(it);
            while (++it2 != _nonterminalRules.constEnd()) {
                if (it.value() == it2.value()) {
                    qDebug() << "same rules"
                             << it.value() << endl
                             << it2.value() << endl;
                    if (_terminalRules.value(it.key()).second
                            == _terminalRules.value(it2.key()).second)
                        sameNT.append(qMakePair(it.key(), it2.key()));
//                    else
//                        qDebug() << "same but different FEATs" << ++kk;
                }
            }

            ++it;
        }
        qDebug() << "same count" << sameNT.size();

        typedef QPair<nonterminal, nonterminal> SameNT;
//        qDebug() << sameNT;
        foreach (const SameNT &same, sameNT) {
            qDebug().noquote() << _nonterminalRules[same.first]
                    << "\tAND\n"
                    << _nonterminalRules[same.second];
        }
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
        _terminalRules.append(QPair<linkID, featureID>(INVALID_LINK, -2)); // occupies size for invalid, and rootNT
        _terminalRules.append(QPair<linkID, featureID>(INVALID_LINK, -1)); // occupies size for invalid, and rootNT
    }

    void addToSrc(nonterminal srcNT, const QList<TDep> &deps)
    {
        RuleBody newRuleBody;

        for (int i=0; i < deps.size(); ++i) {
            nonterminal depNT;
            if (!_terminalRules.contains(deps.at(i).second))
                _terminalRules.append(deps.at(i).second);

            depNT = _terminalRules.index(deps.at(i).second);

            newRuleBody.append(qMakePair(deps.at(i).first, depNT));
        }

        insertRuleBody(srcNT, newRuleBody);
    }

//    void persistentFunction() {
//        // don't delete this, template instantiation

//        dump();
//        load();
//    }

    friend QDataStream &operator<<(QDataStream &ds, const SecondCFG &s);
    friend QDataStream &operator>>(QDataStream &ds, SecondCFG &s);
};

QDataStream &operator<<(QDataStream &ds, const SecondCFG &s) {
    ds << s._terminalRules;
    ds << s._nonterminalRules;
//    ds << s._nextNonterminal;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, SecondCFG &s) {
    ds >> s._terminalRules;
    ds >> s._nonterminalRules;
//    ds >> s._nextNonterminal;

    return ds;
}


#endif // SECONDCFG_H
