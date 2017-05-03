#include "firsttocnfdawg.h"
#include "string.h"
#include "stdio.h"

#include "grammar/humandawg.h"
#include "dawgdic/guide-builder.h"
#include "dawgdic/completer.h"


FirstToCNFDAWG::FirstToCNFDAWG(FirstCFG *cfg)
    : _cfg(cfg)
{
    _cnf.setNextFreeNT(_cfg->terminalRules().size());
}
typedef QPair<RuleDirection, nonterminal> DirectedNonterminal;
typedef QList<DirectedNonterminal> RuleBody;
typedef QList<RuleBody> Rules;

char *directedToKey(DirectedNonterminal dnt)
{
    char *key = nonterminalToKey(dnt.second);
    if (isRight(dnt.first))
        key[3] = key[3] | 0x40;
    return key;
}

char *directedListToKey(const QList<DirectedNonterminal> ldnt)
{
    char *key = new char[ldnt.size() * 4 + 1];
    for (int i=0; i < ldnt.size(); ++i) {
        char *dntKey = directedToKey(ldnt.at(i));
        strcpy(key + i*4, dntKey);
        delete dntKey;
    }
    return key;
}




bool compareStr(const char *lhs, const char *rhs)
{
    return strcmp(lhs, rhs) < 0;
}

void FirstToCNFDAWG::buildFirstCNF()
{
    if (!_cfg)
        return;

    const UniqueVector<nonterminal, featureID> &terminalRules = _cfg->terminalRules();
    const QMap<nonterminal, Rules> &nonterminalRules = _cfg->nonterminalRules();

    QMap<nonterminal, Rules>::const_iterator it(nonterminalRules.constBegin());

    while (it != nonterminalRules.constEnd()) {
        // build dawg for feat
        dawgdic::DawgBuilder dawg_builder;

        QList<char *> keys;
        for (int i=0; i < it.value().size(); ++i) {
            char *key = directedListToKey(it.value()[i]);
            keys.append(key);
        }

        std::sort(keys.begin(), keys.end(), compareStr);
        dawgdic::Dawg dawg;
        dawgdic::Dictionary dic;
        dawgdic::Guide guide;
        for (int i=0; i < keys.size(); ++i) {
            dawg_builder.Insert(keys.at(i),1);
        }
        dawg_builder.Finish(&dawg);
        if (!dawgdic::DictionaryBuilder::Build(dawg, &dic))
            Q_ASSERT(false);
        if (!dawgdic::GuideBuilder::Build(dawg, dic, &guide))
            Q_ASSERT(false);

        dawgdic::Completer completer(dic, guide);

        // translate terrifying optimized dawgdic into nice HumanDAWG
        HumanDAWG hdawg(&completer);
        // so now it may be moved into CNF
        testHDAWG(hdawg, it.value());

        produceRules(it.key(), hdawg);
        qDebug().noquote() << _cnf.toReport();

        ++it;
    }
    replaceOrDuplicateRules();
}

void FirstToCNFDAWG::produceRules(nonterminal fidSrc, HumanDAWG &dawg)
{
    // add terminal rules
    const UniqueVector<nonterminal, featureID> &terminalRules = _cfg->terminalRules();
    for (nonterminal nt = 0; nt < terminalRules.size(); ++nt)
        _cnf.add(nt, terminalRules.value(nt));
    // add binary rules
    dawg.root().prepare();
    _toRepOrDup.clear();

    nonterminal curTopNT = fidSrc;

    produceRules(fidSrc, dawg.root());

}

void FirstToCNFDAWG::produceRules(nonterminal fidSrc, NodeDAWG &node)
{
    if (node.visited)
        return; // done already
    node.visited = true;
    QMap<LabelType, NodeDAWG>::iterator it(node.childs.begin());

    while (it != node.childs.end()) {
        LabelType label = it.key();
        if (!it.value().isEmpty()) {
            nonterminal newNT;
            if (!it.value().visited) {
                it.value().nt = _cnf.nextNT();
            }
            newNT = it.value().nt;

            if (isRight(label.first))
                _cnf.add(fidSrc, newNT, label.second);
            else
                _cnf.add(fidSrc, label.second, newNT);

            produceRules(newNT, it.value());
        }
        if (it.value().final) {
            _toRepOrDup[fidSrc].insert(label.second);
        }

        ++it;
    }
}

void FirstToCNFDAWG::testHDAWG(const HumanDAWG &dawg, const FirstToCNFDAWG::Rules &rules)
{
    foreach (const QList<DirectedNonterminal> &body, rules) {
        if (!dawg.contains(body))
            Q_ASSERT(false);
    }
//    qDebug() << "final count" << dawg.finalCount();
//    qDebug() << "intermediate count" << dawg.intermediateCount();
}

void FirstToCNFDAWG::replaceOrDuplicateRules()
{
    QMap<nonterminal, QSet<nonterminal> >::const_iterator it(_toRepOrDup.constBegin());
    while (it != _toRepOrDup.constEnd()) {
        foreach (nonterminal dest, it.value())
            _cnf.duplicateOrReplace(it.key(), dest);

        ++it;
    }
    _toRepOrDup.clear();
}
