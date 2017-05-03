#include "firstcnf.h"

FirstCNF::FirstCNF()
{

}

void FirstCNF::add(nonterminal nt, featureID t)
{
    _terminalRules.insert(nt, t);
}

void FirstCNF::add(nonterminal src, nonterminal b1, nonterminal b2)
{
    _binaryRules[src].insert(qMakePair(b1, b2));
}

int FirstCNF::binaryRulesCount() const
{
    int result = 0;
    QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > >::const_iterator it(_binaryRules.constBegin());

    while( it != _binaryRules.constEnd()) {
        result += it.value().size();

        ++it;
    }
    return result;
}

void FirstCNF::minimize()
{
//    QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > >::const_iterator it(_binaryRules.constBegin());

//    while( it != _binaryRules.constEnd()) {
//        QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > >::const_iterator it2(it);
//        while ( ++it2 != _binaryRules.constEnd()) {
//            if (it.value() == it2.value()) {
//                if (!_terminalRules.contains(it.key())
//                        && !_terminalRules.contains(it2.key()))
//                    replace(it2.key(), it.key());
//            }

//        }
//        result += it.value().size();

//        ++it;
    //    }
}

void FirstCNF::duplicateOrReplace(nonterminal src, nonterminal dest)
{
    if (_binaryRules.contains(src))
        duplicate(src, dest);
    else
        replace(src, dest);
}

void FirstCNF::replace(nonterminal src, nonterminal dest)
{
    QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > >::iterator it(_binaryRules.begin());

    while( it != _binaryRules.end()) {
        QSet<QPair<nonterminal, nonterminal> >::iterator iset(it.value().begin());
        while(iset != it.value().end()) {
            bool needReplace = false;
            if (iset->first == src || iset->second == src)
                needReplace = true;
            if (needReplace) {
            QPair<nonterminal, nonterminal> newRule =
                    qMakePair((iset->first == src? dest : iset->first),
                              (iset->second == src? dest : iset->second));
            it.value().erase(iset);
            it.value().insert(newRule);
            }
            ++iset;
        }
        ++it;
    }
}

// warning: function doesn't duple S->E, for productions X->SS (it just add X->EE, not X->SE|ES|EE)
void FirstCNF::duplicate(nonterminal src, nonterminal dest)
{
    QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > >::iterator it(_binaryRules.begin());

    while( it != _binaryRules.end()) {
        QSet<QPair<nonterminal, nonterminal> >::iterator iset(it.value().begin());
        while(iset != it.value().end()) {
            bool needDuple = false;
            if (iset->first == src || iset->second == src)
                needDuple = true;
            if (needDuple) {
            QPair<nonterminal, nonterminal> newRule =
                    qMakePair((iset->first == src? dest : iset->first),
                              (iset->second == src? dest : iset->second));
//            it.value().erase(iset);
            it.value().insert(newRule);
            }
            ++iset;
        }
        ++it;
    }
}

QString FirstCNF::toReport() const {
    QString report("terminalCount() %1\n"
                   "binaryRulesCount() %2\n"
                   "nonterminalCount() %3\n");
    return report.arg(terminalCount())
            .arg(binaryRulesCount())
            .arg(nonterminalCount());
}
