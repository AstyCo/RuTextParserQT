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

    _cnf.setTerminalRules(terminalRules);

    QMap<nonterminal, Rules>::const_iterator it(nonterminalRules.constBegin());

    while (it != nonterminalRules.constEnd()) {
        // build dawg for feat
        dawgdic::DawgBuilder dawg_builder;

        QList<char *> keys;
        for (int i=0; i < it.value().size(); ++i) {
//            if (it.key() == _cfg->rootNT()) {
//                if (i != 145 && i != 109 && i != 127)
//                        continue;
//                else {
//                    qDebug() << it.value()[i];
//                }
//            }
            char *key = directedListToKey(it.value()[i]);
            keys.append(key);
//            if (i > 145)
//                break;
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

        if (it.key() == _cfg->rootNT())
            produceRootRules(hdawg);
        else {
            produceRules(it.key(), hdawg);
        }
//        qDebug().noquote() << _cnf.toReport();

        ++it;
    }
    qDebug() << "B1";
//    qDebug() << "B2";
    qDebug().noquote() << _cnf.toReport();
//    qDebug().noquote() << _cnf.toReportRules();

    _cnf.recursiveMinimize();
//    qDebug().noquote() <<"after minimize\n" << _cnf.toReport();
//    qDebug().noquote() << _cnf.toReportRules();
}

void FirstToCNFDAWG::produceRootRules(HumanDAWG &dawg)
{
    // prepare
    dawg.root()->prepare();

    // test HDAWG
    QSharedPointer<NodeDAWG> &root = dawg.root();
    QMap<LabelType, QSharedPointer<NodeDAWG>>::iterator it(root->childs.begin());
    while (it != root->childs.end()) {
        if (it.value()->final) {
            Q_ASSERT(false);
        }
        ++it;
    }
    // produce rules
    produceRootRules(_cfg->rootNT(), root);
}

void FirstToCNFDAWG::produceRootRules(nonterminal fidSrc, QSharedPointer<NodeDAWG> node)
{
    if (node->visited)
        return; // done already
//    qDebug().noquote() << _cnf.toReportRules();
    node->visited = true;
    QMap<LabelType, QSharedPointer<NodeDAWG>>::iterator it(node->childs.begin());
    while (it != node->childs.end()) {
        LabelType label = it.key();
        QSharedPointer<NodeDAWG> childNode = it.value();
        bool needInter = false;
        // add root's end rules
        QMap<LabelType, QSharedPointer<NodeDAWG>>::const_iterator it2(childNode->childs.constBegin());
        while (it2 != childNode->childs.constEnd()) {
            const QSharedPointer<NodeDAWG> &childchildNode = it2.value();
            LabelType childLabel = it2.key();
            if (childchildNode->final) {
                if (isRight(label.first))
                    _cnf.add(fidSrc, childLabel.second, label.second);
                else
                    _cnf.add(fidSrc, label.second, childLabel.second);
            }
            if (!childchildNode->isEmpty())
                needInter = true;
            ++it2;
        }

        // fill intermediate
        if (needInter) {
            // init new nonterminal for node
            if (childNode->nt == -1)
                childNode->nt = _cnf.nextNT();
            nonterminal newNT = childNode->nt;

            // fill intermediate rules
            if (isRight(label.first))
                _cnf.add(fidSrc, newNT, label.second);
            else
                _cnf.add(fidSrc, label.second, newNT);

            // recursion
            produceRootRules(newNT, it.value());
        }
//        else {
//            if (node->nt == -1)
//                node->nt = fidSrc;
//        }

        ++it;
    }
}


void FirstToCNFDAWG::produceRules(nonterminal fidSrc, HumanDAWG &dawg)
{
    // add binary rules
    dawg.root()->prepare();

    produceRules(fidSrc, fidSrc, dawg.root());
}

void FirstToCNFDAWG::produceRules(nonterminal mainNT, nonterminal fidSrc, QSharedPointer<NodeDAWG> node)
{
    if (node->visited)
        return; // done already
    node->visited = true;
    QMap<LabelType, QSharedPointer<NodeDAWG>>::iterator it(node->childs.begin());

    while (it != node->childs.end()) {
        LabelType label = it.key();
        const QSharedPointer<NodeDAWG> &childNode = it.value();
        // fill end rules
        if (childNode->final) {
            if (isRight(label.first))
                _cnf.add(fidSrc, mainNT, label.second);
            else
                _cnf.add(fidSrc, label.second, mainNT);
        }
        if (!childNode->isEmpty()) {
            // init new nonterminal for node
            if (it.value()->nt == -1)
                it.value()->nt = _cnf.nextNT();
            nonterminal newNT = it.value()->nt;

            // fill intermediate rules
            if (isRight(label.first))
                _cnf.add(fidSrc, newNT, label.second);
            else
                _cnf.add(fidSrc, label.second, newNT);

            // recursion
            produceRules(mainNT, newNT, it.value());
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

//void FirstToCNFDAWG::replaceOrDuplicateRules()
//{
//    QMap<nonterminal, QSet<nonterminal> >::const_iterator it(_toRepOrDup.constBegin());
//    while (it != _toRepOrDup.constEnd()) {
//        foreach (nonterminal dest, it.value())
//            _cnf.duplicateOrReplace(it.key(), dest);

//        ++it;
//    }
//    _toRepOrDup.clear();
//}
