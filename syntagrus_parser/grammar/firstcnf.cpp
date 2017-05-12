#include "firstcnf.h"

FirstCNF::FirstCNF()
    : Dumpable("firstcnf.dump")
{

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
    fillAppearance();
    QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > >::iterator it(_binaryRules.begin());

    int a = 0, b = 0;
    while( it != _binaryRules.end()) {
        ++a;
        if (a % 200 == 0)
            qDebug() << "a" << a;

        QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > >::iterator it2(it);
        ++it2;
        while ( it2 != _binaryRules.end()) {
            if (it.value() == it2.value()) {
                it2 = replace(it2, it);
            }
            else {
                ++it2;
            }
        }

        ++it;
    }
}

void FirstCNF::recursiveMinimize()
{
    int iter = 0;
    int size;
    do {
        size = binaryRulesCount();
        minimize();
        if (iter++)
            qDebug() << "ITERATION" << iter;
    }
    while (size != binaryRulesCount());
    if (iter > 1)
        qDebug() << "FINISHED BY" << iter-1 << "ITERATIONS";
}

FirstCNF::BinaryRuleIterator FirstCNF::replace(FirstCNF::BinaryRuleIterator &it2, FirstCNF::BinaryRuleIterator &it)
{
    replace(it2.key(), it.key());

    return _binaryRules.erase(it2);
}

//void FirstCNF::duplicateOrReplace(nonterminal src, nonterminal dest)
//{
//    if (_binaryRules.contains(src))
//        duplicate(src, dest);
//    else
//        replace(src, dest);
//}

void FirstCNF::replace(nonterminal src, nonterminal dest)
{
    const QSet<nonterminal> &set = _hashRuleAppearance[src];
    QSet<nonterminal>::const_iterator itLook(set.constBegin());
    while (itLook != set.constEnd()) {
        if (_binaryRules.contains(*itLook)) {
            QSet<QPair<nonterminal, nonterminal> > &ruleSet = _binaryRules[*itLook];
            QSet<QPair<nonterminal, nonterminal> >::iterator iset(ruleSet.begin());
            while(iset != ruleSet.end()) {
                bool needReplace = false;
                if (iset->first == src || iset->second == src)
                    needReplace = true;
                if (needReplace) {
                    QPair<nonterminal, nonterminal> newRule =
                            qMakePair((iset->first == src? dest : iset->first),
                                      (iset->second == src? dest : iset->second));

                    iset = ruleSet.erase(iset);
                    ruleSet.insert(newRule);
                }
                else
                    ++iset;
            }
        }

        ++itLook;
    }
}

void FirstCNF::fillAppearance()
{
    _hashRuleAppearance.clear();

    QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > >::const_iterator it(_binaryRules.constBegin());
    while( it != _binaryRules.constEnd()) {
        QSet<QPair<nonterminal, nonterminal> >::const_iterator iset(it.value().constBegin());
        while(iset != it.value().end()) {
            _hashRuleAppearance[iset->first].insert(it.key());
            _hashRuleAppearance[iset->second].insert(it.key());

            ++iset;
        }

        ++it;
    }
    qDebug() << "hash filled";
}

//// warning: function doesn't duple S->E, for productions X->SS (it just add X->EE, not X->SE|ES|EE)
//void FirstCNF::duplicate(nonterminal src, nonterminal dest)
//{
//    QMap<nonterminal, QSet<QPair<nonterminal, nonterminal> > >::iterator it(_binaryRules.begin());

//    while( it != _binaryRules.end()) {
//        QSet<QPair<nonterminal, nonterminal> >::iterator iset(it.value().begin());
//        while(iset != it.value().end()) {
//            bool needDuple = false;
//            if (iset->first == src || iset->second == src)
//                needDuple = true;
//            if (needDuple) {
//            QPair<nonterminal, nonterminal> newRule =
//                    qMakePair((iset->first == src? dest : iset->first),
//                              (iset->second == src? dest : iset->second));
////            it.value().erase(iset);
//            it.value().insert(newRule);
//            }
//            ++iset;
//        }
//        ++it;
//    }
//}

QString FirstCNF::toReport() const {
    QString report("terminalCount() %1\n"
                   "binaryRulesCount() %2\n"
                   "nonterminalCount() %3\n");
    return report.arg(terminalCount())
            .arg(binaryRulesCount())
            .arg(nonterminalCount());
}

QString FirstCNF::toReportRules() const {
    QString rules;
    rules += QString("terminalCount %1\n").arg(terminalCount());

    BinaryRules::const_iterator it(_binaryRules.constBegin());
    while (it != _binaryRules.constEnd()) {
        rules += QString("S%1->").arg(it.key());
        QSet<QPair<nonterminal, nonterminal> >::const_iterator iset(it.value().constBegin());
        bool first = true;
        while (iset != it.value().constEnd()) {
            rules += QString (" %3S%1 S%2").arg(iset->first).arg(iset->second)
                    .arg((first?"":"| "));

            if (first)
                first = false;
            ++iset;
        }
        rules += "\n";

        ++it;
    }
    return rules;
}






