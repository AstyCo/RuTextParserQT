#include "tst_all.h"

#include "dawgdic/dawg-builder.h"
#include "dawgdic/dictionary-builder.h"
#include "morphclient.h"
#include "grammar/firstcfgparser.h"
#include "grammar/firsttocnfdawg.h"


#include <fstream>
#include <iostream>
#include <algorithm>

#include <QStringBuilder>

#define UPPER_MASK 0xFF00
#define LOWER_MASK 0xFF


void SynTagRusParserTest::test()
{
//    QBENCHMARK {

    _syntagrusParser.deserializeTreeCorpora();
    qDebug() << "Size of TreeCorpora after deserialization" << _syntagrusParser.getTreeCorpora()->size();
    _grammarParser.deserializeGrammar();
    qDebug() << "Grammar size after serialization" << _grammarParser.getGrammar()->size();



//    testCYKSyntacticalAnalyzer();

//    }
}

void SynTagRusParserTest::parsingTest()
{
    _syntagrusParser.parse();

    // test TreeCorpora
    {
        QVERIFY2(!_syntagrusParser.getTreeCorpora()->isEmpty(), "TreeCorpora is empty after parsing");
        qDebug() << _syntagrusParser.getTreeCorpora()->size();
    }

    OptimizedTreeCorpora optimized(*_syntagrusParser.getTreeCorpora());

    {
        QVERIFY2(!optimized.multihashSentences().isEmpty(), "OptimizedTreeCorpora is empty after initialization");

        qDebug() << "features size" << optimized.featureMapper().size();
        optimized.serialize();
    }
}

bool lessThanSentences(const OptimizedSentence &s1, const OptimizedSentence &s2)
{
    for (int i=0; i < s2.words().size(); ++i) {
        if (i >= s1.words().size())
            return true;
        if (s1.words().at(i) == s2.words().at(i))
            continue;
        return s1.words().at(i).getFeature() < s2.words().at(i).getFeature();
    }
    return false;
}


void SynTagRusParserTest::dawgTest()
{
    OptimizedTreeCorpora optimized;
    optimized.deserialize();

    qDebug() << "Optimized after deserialization" << optimized.multihashSentences().size();

    // Init sentences
    QVector<OptimizedSentence> sortedSentences(optimized.multihashSentences().size());
    QMultiHash<int, OptimizedSentence>::const_iterator i(optimized.multihashSentences().constBegin());
    int index = 0;
    while ( i!= optimized.multihashSentences().constEnd()) {
        sortedSentences[index] = i.value();

        ++index;
        ++i;
    }
    // Sort sentences
    std::sort(sortedSentences.begin(), sortedSentences.end(), lessThanSentences);

    // Insert keys into DAWG
    dawgdic::DawgBuilder dawgBuilder;
    for (int i=0; i < sortedSentences.size(); ++i) {
        // Initialize key from OptimizedWord
        char *key = sortedSentences.at(i).toKey();
        // Insert
        bool inserted = dawgBuilder.Insert(key, sortedSentences.at(i).words().first().size()); // value = count of occuriences
        if (!inserted) {
            Q_ASSERT(false);
        }
        if (key)
            delete []key;
    }
    // Finish DAWG
    dawgdic::Dawg dawg;
    dawgBuilder.Finish(&dawg);

    // Builds a dictionary from a simple dawg.
    dawgdic::Dictionary dic;
    dawgdic::DictionaryBuilder::Build(dawg, &dic);

    qDebug() << "dawgdic::Dictionary size" << dic.file_size();

    // Writes a dictionary into a file "test.dic".
    std::ofstream dic_file("test.dic", std::ios::binary);
    dic.Write(&dic_file);

}

void SynTagRusParserTest::dawgLoadTest()
{
    OptimizedTreeCorpora optimized;
    optimized.deserialize();

    // Reads a dictionary from a file "test.dic".
    std::ifstream dic_file("test.dic", std::ios::binary);
    dawgdic::Dictionary dic;
    dic.Read(&dic_file);

    qDebug() << "dictionary loaded";

    QMultiHash<int, OptimizedSentence>::const_iterator i25(optimized.multihashSentences().constFind(25));
    QBENCHMARK {
        if (i25 != optimized.multihashSentences().constEnd()) {
            char *key = i25.value().toKey();
            Q_ASSERT(key);
            qDebug() << i25.value() << "found?" << dic.Contains(key);
            if (key)
                delete []key;
        }
        int rand = qAbs(qrand()) % 30 + 1;
        QMultiHash<int, OptimizedSentence>::const_iterator i(optimized.multihashSentences().constFind(rand));
        if (i != optimized.multihashSentences().constEnd()) {
            char *key = i.value().toKey();
            Q_ASSERT(key);
            qDebug() << rand << ':' << i.value() << "found?" << dic.Contains(key);
            if (key)
                delete []key;
        }
    }
}


void SynTagRusParserTest::simpleTestMorphClient()
{
    qDebug() << "analyzed" << _client.analyzeWord(QString::fromUtf8("Арбуз"));

    QStringList sentence;
    sentence << QString::fromUtf8("Однако")
             << QString::fromUtf8("Однако")
             << QString::fromUtf8("Семён")
             << QString::fromUtf8("Еремеевич")
             << QString::fromUtf8("загруженный")
             << QString::fromUtf8("текучкой")
             << QString::fromUtf8("не")
             << QString::fromUtf8("успел")
             << QString::fromUtf8("сделать")
             << QString::fromUtf8("это");

    qDebug() << "analyzed2" << _client.analyze(sentence);

//    client.analyzeWord(QString::fromUtf8("дыня"));
//    client.analyzeWord(QString::fromUtf8("помидорка"));
//    client.analyzeWord(QString::fromUtf8("есть"));

}

void SynTagRusParserTest::testMorphClient()
{
    _optimized.deserialize();

    const QMultiHash<int, OptimizedSentence> &sentences = _optimized.multihashSentences();

    QMultiHash<int, OptimizedSentence>::const_iterator it(sentences.constBegin());
    while (it != sentences.constEnd()) {
        if (_client.state() != QAbstractSocket::ConnectedState) {
            qDebug() << "Socket not connected";
            return;
        }
        for (int inn=0; inn < it.value().size(); ++inn) {
            morphClientTestSentence(it.value(), inn);
        }
        it++;
    }

}

void SynTagRusParserTest::testFirstGrammar()
{
//    parsingTest();
    deserializeTreeCorpora();

    FirstCFGParser parser("cfg1.dump");

    parser.parseTree(*_syntagrusParser.getTreeCorpora());

    FirstCFG *cfg = parser.getCFG();
//    cfg->load();
    qDebug().noquote() << cfg->toReport();
    cfg->dump();
}

void SynTagRusParserTest::testFirstGrammar2()
{
    FirstCFG cfg("cfg1.dump");
    cfg.load();
    qDebug().noquote() << cfg.toReport();

    FirstToCNFDAWG toDawg(&cfg);
    toDawg.buildFirstCNF();

    qDebug() << "finished";
    qDebug().noquote() << toDawg.cnf().toReport();
    toDawg.cnf().dump();
}

#include "grammar/secondcfgparser.h"
void SynTagRusParserTest::testSecondGrammar()
{
//    parsingTest();
    deserializeTreeCorpora();

    FirstCFG firstCFG("cfg1.dump");
    firstCFG.load();
    SecondCFGParser parser("cfg2.dump");
    parser.setFirstCFG(&firstCFG);
    parser.parseTree(*_syntagrusParser.getTreeCorpora());

    SecondCFG *cfg = parser.getCFG();
//    cfg->load();
    qDebug().noquote() << cfg->toReport();
    cfg->dump();
}

#include "grammar/firsttocnfdawg.h"
void SynTagRusParserTest::testSecondGrammar2()
{
    SecondCFG cfg("cfg2.dump");
    cfg.load();
    qDebug().noquote() << cfg.toReport();


    cfg.checkSameRules();
}

void SynTagRusParserTest::testLogic()
{
    FirstCFG cfg("nodumb.dump");
    typedef QPair<RuleDirection, QPair<linkID, featureID> > TDep;

    featureID S = 100;
    TDep AL = qMakePair(produceDirection(false), qMakePair<linkID, featureID>(100, 2));
    TDep BL = qMakePair(produceDirection(false), qMakePair<linkID, featureID>(101, 3));
    TDep CL = qMakePair(produceDirection(false), qMakePair<linkID, featureID>(109, 4));
    TDep DL = qMakePair(produceDirection(false), qMakePair<linkID, featureID>(106, 5));
    TDep EL = qMakePair(produceDirection(false), qMakePair<linkID, featureID>(105, 6));
    TDep FL = qMakePair(produceDirection(false), qMakePair<linkID, featureID>(110, 7));


    TDep AR = qMakePair(produceDirection(true), qMakePair<linkID, featureID>(107, 2));
    TDep BR = qMakePair(produceDirection(true), qMakePair<linkID, featureID>(108, 3));
    TDep CR = qMakePair(produceDirection(true), qMakePair<linkID, featureID>(102, 4));
    TDep DR = qMakePair(produceDirection(true), qMakePair<linkID, featureID>(103, 5));
    TDep ER = qMakePair(produceDirection(true), qMakePair<linkID, featureID>(104, 6));
    TDep FR = qMakePair(produceDirection(true), qMakePair<linkID, featureID>(111, 7));





    QList<TDep> rule1, rule2, rule3, rule4, rule5, rule6;

//    rule1 << AL << BL << CR << DR << ER;
//    rule3 << AL << BL << DR << DR << ER;
//    rule5 << AL << BL << CR << AR << ER;
//    rule6 << AL << BL << DR << AR << ER;


//    rule2 << AL << BL << EL << DL << CR;
//    rule4 << AL << BL << EL << DL << DR;

    rule1 << AR;
    rule2 << CL << ER;
    rule3 << ER << AR;

    cfg.addRoot(BR.second.second, rule1);
    cfg.addRoot(FR.second.second, rule2);
    cfg.addRoot(FR.second.second, rule3);

    //simple test

//    cfg.addRoot(S, rule4);

//    cfg.addRoot(S, rule5);
//    cfg.addRoot(S, rule6);



    FirstToCNFDAWG builder(&cfg);
    builder.buildFirstCNF();

    qDebug() << "finished";
    qDebug().noquote() << builder.cnf().toReport();

}
#include "grammar/simplycnfbuilder.h"
void SynTagRusParserTest::testSimpleFirst()
{
    FirstCFG cfg("cfg1.dump");
    cfg.load();
    qDebug().noquote() << cfg.toReport();

    SimplyCNFBuilder builder(&cfg);
    builder.build();

    qDebug() << "finished";
    qDebug().noquote() << builder.cnf().toReport();
}

SynTagRusParserTest::MorphStates SynTagRusParserTest::morphClientTestSentence(const OptimizedSentence &opt, const int &inn)
{
    static const int minSz = 5, maxSz = 30;
    static long totalCount = 0;
    static long failedCount = 0;
    static long skippedCount = 0;
    static long suc = 0;
    static long sucAtFirst = 0;
    bool failedFirst = false;
    bool failed = false;
    bool skipped = false;
    if (opt.words().size() < minSz || opt.words().size() > maxSz)
        return SynTagRusParserTest::skippedState;

    ++totalCount;
    for (int i=0; i<opt.words().size(); ++i) {
        const OptimizedWord &word = opt.words().at(i);
        switch (morphClientTestWord(word, inn)) {
        case successedNotFirstState:
            failedFirst = true;
            break;
        case failedState:
            failed = true;
            break;
        case skippedState:
            skipped = true;
            break;
        default:
            break;
        }
    }
    if (skipped) {
        skippedCount++;
        logStream << QString("[%1/%2/%3/%4/%5]: ")
                     .arg(totalCount)
                     .arg(QString::number(suc))
                     .arg(sucAtFirst)
                     .arg(skippedCount)
                     .arg(QString::number(failedCount))
                 << "failed sentence\n\t"
                 << opt.getSentence(inn).join(" ")
                 << endl;
    }
    else if (failed) {
        failedCount++;
        logStream << QString("[%1/%2/%3/%4/%5]: ")
                     .arg(totalCount)
                     .arg(QString::number(suc))
                     .arg(sucAtFirst)
                     .arg(skippedCount)
                     .arg(QString::number(failedCount))
                 << "failed sentence\n\t"
                 << opt.getSentence(inn).join(" ")
                 << endl;
    }
    else {
        suc++;
        if (!failedFirst)
            sucAtFirst++;
        extraLogStream << QString("[%1/%2/%3/%4/%5]: ")
                          .arg(totalCount)
                          .arg(QString::number(suc))
                          .arg(sucAtFirst)
                          .arg(skippedCount)
                          .arg(QString::number(failedCount))
                 << "success sentence\n\t"
                 << opt.getSentence(inn).join(" ")
                 << endl;
    }

    return (skipped ? skippedState : (failed ? failedState : successedNotFirstState));
}

inline QString toString(const MorphResultType &analyzed) {
    QString result = "MORPH {";

    for (int i=0; i < analyzed.size(); ++i) {
        result = result
                % "\n\tQPair(\'"
                % analyzed.at(i).first
                % "\', "
                % QString::number(analyzed.at(i).second)
                % ')';
    }
    result = result % "\n}";
    return result;
}

SynTagRusParserTest::MorphStates SynTagRusParserTest::morphClientTestWord(const OptimizedWord &word, const int &inn)
{
    static long totalCount = 0;
    static long failedCount = 0;
    static long skippedCount = 0;
    static long successfulCount = 0;
    static long firstFoundCount = 0;
    ++totalCount;
    int foundAt = -1;

    QString feature = _optimized.featureMapper().value(word.feature);
    MorphResultType analyzed = _client.analyzeWord(word.words[inn]);

    for (int i=0; i < analyzed.size(); ++i) {
        if (analyzed.at(i).first == feature)
            // found
            foundAt = i;
    }

    QString what;
    if (foundAt == -1) {
        if (analyzed.isEmpty()) {
            ++skippedCount;
            what = "skipped";
        }
        else {
            ++failedCount;
            what = "failed";
        }
//        Q_ASSERT(failedCount < 100);
        logStream << QString("[%1/%2/%3/%4/%5]:")
                    .arg(totalCount)
                    .arg(QString::number(successfulCount))
                    .arg(firstFoundCount)
                    .arg(skippedCount)
                    .arg(QString::number(failedCount))
                 << QString(" %1 word\n\t").arg(what)
                 << word.words.at(inn) << ' ' << feature
                 << endl;
        logStream << toString(analyzed)
                  << endl;
    }
    else {
        successfulCount++;
        if (foundAt == 0)
            firstFoundCount++;
//        Q_ASSERT(successfulCount < 1000);
        extraLogStream <<  QString("[%1/%2/%3/%4/%5]:")
                           .arg(totalCount)
                           .arg(QString::number(successfulCount))
                           .arg(firstFoundCount)
                           .arg(skippedCount)
                           .arg(QString::number(failedCount))
                     << " success word\n\t"
                     << word.words.at(inn) << ' ' << feature
                     << endl;
        extraLogStream << toString(analyzed)
                  << endl;
    }

    if (what == "skipped")
        return skippedState;
    if (what == "failed")
        return failedState;
    if (foundAt == 0)
        return successedAtFirstState;
    return successedNotFirstState;
}

const FeatureMapper *_dirtyHack;
const LinkMapper *_dirtyHack2;
struct HelperCompare
{
    QString feat;
    QString link;
    bool isRight;
    int index;
};

bool lessThan2(HelperCompare lhs, HelperCompare rhs)
{
    if (_dirtyHack->index(lhs.feat) < _dirtyHack->index(rhs.feat))
        return true;
    if (_dirtyHack->index(lhs.feat) > _dirtyHack->index(rhs.feat))
        return false;
    return _dirtyHack2->index(lhs.link) < _dirtyHack2->index(rhs.link);
}
bool lessThan(RecordNode *lhs, RecordNode *rhs)
{
    if (_dirtyHack->index(lhs->record()._feat)
            < _dirtyHack->index(rhs->record()._feat))
        return true;
    if (_dirtyHack->index(lhs->record()._feat)
            > _dirtyHack->index(rhs->record()._feat))
        return false;
    return _dirtyHack2->index(lhs->record()._link)
            < _dirtyHack2->index(rhs->record()._link);
}


bool SynTagRusParserTest::checkEqual(RuleNode *ruleNode, RecordNode *recordNode,
                                     const QVector<ScoredChomskyRuleRecord> &ruleByID,
                                     const FeatureMapper &fmapper, const LinkMapper &lmapper)
{
    if (!ruleNode && !recordNode)
        return true;
    if (!ruleNode || !recordNode)
        return false;

    if (recordNode->childNodes().size() != ruleNode->rules().size())
        return false;
    if (ruleNode->rules().isEmpty()
            && recordNode->childNodes().isEmpty())
        return true;
//    qDebug().noquote() << ruleNode->toString(ruleByID, fmapper, lmapper);
    const RecordInCorpora &record = recordNode->record();
    const ChomskyRuleRecord &ruleGrammar = ruleByID.at(ruleNode->rules().first().id).rule;
    QString srcFeature = fmapper.value(ruleGrammar._sourceFID);
    if (srcFeature != record._feat) {
//        qDebug() << "!check feats" << srcFeature << record._feat;
        return false;
    }
    const QList<RecordNode *> &sortedChilds = recordNode->childNodes();
//    _dirtyHack = &fmapper;
//    _dirtyHack2 = &lmapper;

//    std::sort(sortedChilds.begin(), sortedChilds.end(), lessThan);
    int j =0;
    for (; j < ruleNode->rules().size(); ++j) {
        const RuleLink &childLink = ruleNode->rules().at(j);
        HelperCompare helper;
        const ChomskyRuleRecord &rule = ruleByID.at(childLink.id).rule;

        if (!rule._isRightRule)
            break;
    }
    // j == first left rule
    QList<HelperCompare> temp;
    for (int i=ruleNode->rules().size() - 1; i >= j ; --i) {
        const RuleLink &childLink = ruleNode->rules().at(i);
        HelperCompare helper;
        const ChomskyRuleRecord &rule = ruleByID.at(childLink.id).rule;

        helper.isRight = rule._isRightRule;
        helper.link = lmapper.value(rule._linkID);
        helper.feat = fmapper.value(rule._dependFID);
        helper.index = i;

        temp.append(helper);
    }

    for (int i=0; i < j; ++i) {
        const RuleLink &childLink = ruleNode->rules().at(i);
        HelperCompare helper;
        const ChomskyRuleRecord &rule = ruleByID.at(childLink.id).rule;

        helper.isRight = rule._isRightRule;
        helper.link = lmapper.value(rule._linkID);
        helper.feat = fmapper.value(rule._dependFID);
        helper.index = i;

        temp.append(helper);
    }



//    std::sort(temp.begin(), temp.end(), lessThan2);


    for (int i=0; i < sortedChilds.size(); ++i) {
        const RecordInCorpora &childRecord = sortedChilds.at(i)->record();

        if (temp.at(i).isRight != recordNode->record().before(childRecord)) {
            return false;
        }

        if (temp.at(i).link != childRecord._link) {
//            qDebug() << "links not equal" << temp.at(i).link
//                     << "and" << childRecord._link;
            return false;
        }
        if (temp.at(i).feat != childRecord._feat){
            qDebug() << "feats not equal" << temp.at(i).feat
                     << "and" << childRecord._feat;
            return false;
        }
        if (!checkEqual(ruleNode->rules().at(temp.at(i).index).node.data(), sortedChilds.at(i),ruleByID, fmapper, lmapper))
            return false;
//        logStream << ruleNode->toString(ruleByID, fmapper, lmapper);
    }
    return true;
}

QList<QPair<featureID, linkID> > SynTagRusParserTest::toLAS(const RecordNode *node,
                                                            const FeatureMapper &fmapper, const LinkMapper &lmapper) const
{
    QPair<QList<QPair<featureID, linkID> >, QList<QPair<featureID, linkID> > > tmp = toLASH(node, fmapper, lmapper);
    tmp.first.append(tmp.second);
    return tmp.first;
}

QPair<QList<QPair<featureID, linkID> >, QList<QPair<featureID, linkID> > > SynTagRusParserTest::toLASH(const RecordNode *node, const FeatureMapper &fmapper, const LinkMapper &lmapper) const
{
    QPair<QList<QPair<featureID, linkID> >, QList<QPair<featureID, linkID> > > result;
    const RecordInCorpora &parentRecord = node->record();

    for (int i=0; i < node->childNodes().size(); ++i) {
        const RecordNode *child = node->childNodes()[i];
        const RecordInCorpora &childRecord = child->record();

        QPair<QList<QPair<featureID, linkID> >,QList<QPair<featureID, linkID> > > tmp
                = toLASH(child, fmapper, lmapper);

        if (parentRecord.before(childRecord)){
            result.second.append(tmp.first);
            result.second.append(qMakePair(fmapper.index(parentRecord._feat), lmapper.index(childRecord._link)));
            result.second.append(tmp.second);
        }
        else {
            result.first.append(tmp.first);
            result.first.append(qMakePair(fmapper.index(parentRecord._feat), lmapper.index(childRecord._link)));
            result.first.append(tmp.second);
        }
    }
    return result;
}

QList<featureID> SynTagRusParserTest::toUAS(const RecordNode *node, const FeatureMapper &fmapper, const LinkMapper &lmapper) const
{
    QList<QPair<featureID, linkID> > tmp = toLAS(node, fmapper, lmapper);
    QList<featureID> result;
    for (int i=0; i<tmp.size(); ++i)
        result.append(tmp.at(i).first);
    return result;
}

int SynTagRusParserTest::calcLASForSent(const RuleNode *rulenode, const RecordNode *recordnode, const CNFGrammar &grammar, const FeatureMapper &fmapper, const LinkMapper &lmapper) const
{
    int result = 0;
    QList<QPair<featureID, linkID> > foundLAS = rulenode->toLAS(grammar);
    QList<QPair<featureID, linkID> > needLAS = toLAS(recordnode, fmapper, lmapper);

    Q_ASSERT(foundLAS.size() == needLAS.size());
    for (int i=0; i<foundLAS.size(); ++i) {
        if (foundLAS.at(i) == needLAS.at(i))
            result++;
    }
    return result;
}

int SynTagRusParserTest::calcUASForSent(const RuleNode *rulenode, const RecordNode *recordnode, const CNFGrammar &grammar, const FeatureMapper &fmapper, const LinkMapper &lmapper) const
{
    int result = 0;
    QList<featureID> foundUAS = rulenode->toUAS(grammar);
    QList<featureID> needUAS = toUAS(recordnode, fmapper, lmapper);

    Q_ASSERT(foundUAS.size() == needUAS.size());
    for (int i=0; i<foundUAS.size(); ++i) {
        if (foundUAS.at(i) == needUAS.at(i))
            result++;
    }
    return result;
}


void SynTagRusParserTest::deserializeTreeCorpora()
{
    if (treeCorporaLoaded)
        return;
    _syntagrusParser.deserializeTreeCorpora();
    treeCorporaLoaded = true;
}

void SynTagRusParserTest::deserializeGrammar()
{
    if (grammarLoaded)
        return;
    _grammarParser.deserializeGrammar();
    grammarLoaded = true;
}

void SynTagRusParserTest::treeCorporaSerializationTest()
{
    TreeCorpora *treeCorpora = _syntagrusParser.getTreeCorpora();
    long long sizeBefore = treeCorpora->size();

    _syntagrusParser.serializeTreeCorpora();
    deserializeTreeCorpora();

    qDebug() << "size after deserialization" << _syntagrusParser.getTreeCorpora()->size();

     QVERIFY2(sizeBefore == _syntagrusParser.getTreeCorpora()->size(),
              "TreeCorpora of different size after serialization/deserialization");
}

void SynTagRusParserTest::grammarTest()
{
    deserializeTreeCorpora();
    _grammarParser.parseTree(*_syntagrusParser.getTreeCorpora());

    QString report = _grammarParser.getGrammar()->toReport(_syntagrusParser.getTreeCorpora()->featureMapper());
    qDebug() << "Grammar before serialization" << report;

    _grammarParser.serializeGrammar();
    deserializeGrammar();

    qDebug() << "Grammar after deserialization" << _grammarParser.getGrammar()->toReport(_syntagrusParser.getTreeCorpora()->featureMapper());
    QVERIFY2(report == _grammarParser.getGrammar()->toReport(_syntagrusParser.getTreeCorpora()->featureMapper()),
             "Grammar different after serialization/deserialization");
}

AmbigiousStringVector toAmbigious(const SentenceInfo &sentence) {
    AmbigiousStringVector res;

    foreach (const WordInCorpora &word, sentence.vector()){
        QList<QStringList> wlist;
        wlist.append(QStringList(word.feature()));
        res.append(wlist);
    }
    return res;
}

const CNFGrammar *hack;
const FeatureMapper *fmapperHack;
const LinkMapper *lmapperHack;
bool compareByProb(QSharedPointer<RuleNode> lhs, QSharedPointer<RuleNode> &rhs)
{
    return lhs->calcProb(-1, *hack, *fmapperHack, *lmapperHack)
            > rhs->calcProb(-1, *hack, *fmapperHack, *lmapperHack);
}

bool compareByProb21(QSharedPointer<RuleNode> lhs, QSharedPointer<RuleNode> &rhs)
{
    return lhs->calcProb2(-1, *hack, *fmapperHack, *lmapperHack)
            > rhs->calcProb2(-1, *hack, *fmapperHack, *lmapperHack);
}
bool compareByProb22(QSharedPointer<RuleNode> lhs, QSharedPointer<RuleNode> &rhs)
{
    return lhs->calcProb2(-1, *hack, *fmapperHack, *lmapperHack)
            > rhs->calcProb2(-1, *hack, *fmapperHack, *lmapperHack);
}





void SynTagRusParserTest::testCYKSyntacticalAnalyzer()
{
    static int totalSentences = 0;
    static int doneSentences = 0;
//    return;
    ExtensionsLogs::Logs::registerLogStream("info.n++");
//    parsingTest();
    deserializeTreeCorpora();
//    SynTagRusParser parser;
//    parser.deserializeTreeCorpora();
//    parser.parse("../../rutextparserqt/test");
//    qDebug() << "sent count:" << parser.getTreeCorpora()->sentencesBySize().size();
    deserializeGrammar();
    CNFGrammar *grammar=_grammarParser.getGrammar();
    grammar->calcSimpleRuleTrees();
    QString result = grammar->toReport(_syntagrusParser.getTreeCorpora()->featureMapper());
//    qDebug().noquote() << result;
    *ExtensionsLogs::Logs::log("info.n++") << result;
//    return;
    grammar->rulesByFeatureID().size();
    FeatureMapper fmapper;
    LinkMapper lmapper;
    fmapper.load();
    lmapper.load();
    CYKSyntacticalAnalyzer an(fmapper, lmapper);
    an.setMaxTime(0);
//    return;
    const int sz = 15, minSz = 5, maxSz = 20;
    QMultiHash<int, SentenceInCorpora> sentences(_syntagrusParser.getTreeCorpora()->sentencesBySize());
//    QMultiHash<int, SentenceInCorpora> sentences(parser.getTreeCorpora()->sentencesBySize());
    QMultiHash<int, SentenceInCorpora>::const_iterator it(sentences.constBegin());
    int first =0, firstP = 0, second = 0, secondP = 0, total=0;
    int firstUAS=0, firstLAS=0, firstTotal=0;
    int secondUAS=0, secondLAS=0, secondTotal=0;
    hack = grammar;
    fmapperHack = &fmapper;
    lmapperHack = &lmapper;
    while(it!=sentences.constEnd()) {
        if (it.key() < minSz || it.key() > maxSz) {
            ++it;
            continue;
        }

        SentenceInCorpora sentence = it.value();
        qDebug() << tr("sentence (%1, %2)").arg(sentence.getFilename()).arg(sentence.getId())
                 << sentence.qDebugSentence().wordList()
                 << '\n' << sentence.qDebugSentence().featList();
        if (!sentence.isProjective()) {
            qDebug() << "NOT PROJECTIVE";
            it++;
            continue;
        }



        qDebug() << "starting analyzing";

        QString error;
        QList<QSharedPointer<RuleNode> > analyzedSentences = an.analyze(toAmbigious(sentence.qDebugSentence()),
                                                                        *_grammarParser.getGrammar(), &error);
        ++totalSentences;
        firstTotal+=sentence.size();

        if (!error.isEmpty()) {
            logStream << QString("Error(%1) in sentence of size %2")
                         .arg(error)
                         .arg(analyzedSentences.size())
                      << endl;
            continue;
        }
        ++doneSentences;
        logStream << QString("[%1][%2] DONE SENTENCE OF SIZE %3")
                     .arg(totalSentences)
                     .arg(doneSentences)
                     .arg(analyzedSentences.size())
                  << endl;

        total += analyzedSentences.size();
        {
            QList<QSharedPointer<RuleNode> > analyzedSentences21 = analyzedSentences;
            std::sort(analyzedSentences21.begin(), analyzedSentences21.end(), compareByProb21);
            int found = 0;
            for (int i=0; i < analyzedSentences21.size(); ++i) {
                QSharedPointer<RuleNode> rn = analyzedSentences21.at(i);
                bool founded = false;
                if (checkEqual(rn.data(), sentence.root(), _grammarParser.getGrammar()->rulesByID(), fmapper, lmapper)) {
                    found++;
                    founded = true;
                    *ExtensionsLogs::Logs::log("info.n++") << QString("First Found at %1 of %2").arg(i).arg(analyzedSentences21.size()) << endl;
                    first+= i;
                    if (i==0)
                        firstP++;
                    firstLAS += calcLASForSent(rn.data(), sentence.root(),
                                               *_grammarParser.getGrammar(), fmapper, lmapper);
                    firstUAS += calcUASForSent(rn.data(), sentence.root(),
                                               *_grammarParser.getGrammar(), fmapper, lmapper);

                }
                logStream << QString("[%1](%2) ")
                             .arg(i)
                             .arg(rn->calcProb2(-1, *grammar, fmapper, lmapper))
                          << (founded ? "FOUNDED:\n" : "\n") << QString(" of %1 ").arg(analyzedSentences21.size())
//                          << rn->toString( _grammarParser.getGrammar()->rulesByID(), fmapper, lmapper)
                          << endl;
            }
            if (found)
                qDebug() << "NICE, FOUND" << found;
            else
                qDebug() << "SAD, NOT FOUND";
            Q_ASSERT(found == 1);
            QVERIFY2(!analyzedSentences21.isEmpty(), "not found");
        }

        secondTotal += sentence.size();
        std::sort(analyzedSentences.begin(), analyzedSentences.end(), compareByProb);
        int found = 0;
        for (int i=0; i < analyzedSentences.size(); ++i) {
            QSharedPointer<RuleNode> rn = analyzedSentences.at(i);
            bool founded = false;
            if (checkEqual(rn.data(), sentence.root(), _grammarParser.getGrammar()->rulesByID(), fmapper, lmapper)) {
                found++;
                founded = true;
                *ExtensionsLogs::Logs::log("info.n++") << QString("Third Found at %1 of %2").arg(i).arg(analyzedSentences.size()) << endl;
                second+= i;
                if (i==0)
                    secondP++;
                secondLAS += calcLASForSent(rn.data(), sentence.root(),
                                           *_grammarParser.getGrammar(), fmapper, lmapper);
                secondUAS += calcUASForSent(rn.data(), sentence.root(),
                                           *_grammarParser.getGrammar(), fmapper, lmapper);
            }
            logStream << QString("[%1](%2) ")
                         .arg(i)
                         .arg(rn->calcProb(-1, *grammar, fmapper, lmapper))
                      << (founded ? "FOUNDED:\n" : "\n")<< QString(" of %1 ").arg(analyzedSentences.size())
//                      << rn->toString( _grammarParser.getGrammar()->rulesByID(), fmapper, lmapper)
                      << endl;
        }

        *ExtensionsLogs::Logs::log("info.n++") << QString("Current scores [P: %1 I:%2 UAS:%3 LAS: %4]"
                                                          "[P: %5 I:%6 UAS:%7 LAS: %8]")
                                                  .arg(firstP *1. / totalSentences)
                                                  .arg(first * 1. / total)
                                                  .arg(firstUAS *1. / firstTotal)
                                                  .arg(firstLAS *1. / firstTotal)

                                                  .arg(secondP * 1. / totalSentences)
                                                  .arg(second * 1. / total)
                                                  .arg(secondUAS *1. / secondTotal)
                                                  .arg(secondLAS *1. / secondTotal)

                                               << endl;

        if (found)
            qDebug() << "NICE, FOUND" << found;
        else
            qDebug() << "SAD, NOT FOUND";
        Q_ASSERT(found == 1);
        QVERIFY2(!analyzedSentences.isEmpty(), "not found");
//        qDebug() << "analyzedSentences";
        //    foreach (const QStringList &sl, analyzedSentences)
        //        qDebug() << "\tSentence:" << sl;
        it++;
//        return;
    }
    qDebug() << "all sentences with sz" << sz << "analyzed";
}

void SynTagRusParserTest::testProjective()
{
    if (!ExtensionsLogs::Logs::registerLogStream("info.n++")) {
        Q_ASSERT(false);
    }

    deserializeTreeCorpora();
    TreeCorpora *treeCorpora = _syntagrusParser.getTreeCorpora();

    const QMultiHash<int, SentenceInCorpora> &sentencesBySize = treeCorpora->sentencesBySize();
    QMultiHash<int, SentenceInCorpora>::const_iterator it(sentencesBySize.constBegin());


    QMap<int, QPair<int, int> > projectiveInfo; // size => QPair(count, projective_count)
    while(it != sentencesBySize.constEnd()) {
        if (!projectiveInfo.contains(it.key()))
            projectiveInfo.insert(it.key(), qMakePair(0, 0));

        projectiveInfo[it.key()].first++;
        if (it.value().isProjective())
            projectiveInfo[it.key()].second++;

        ++it;
    }

    int totalSent = 0;
    int totalProjective = 0;
    QMap<int, QPair<int, int> >::const_iterator iterator(projectiveInfo.constBegin());

    while(iterator != projectiveInfo.constEnd()) {
        const QPair<int, int> &val = iterator.value();

        totalSent += val.first;
        totalProjective += val.second;
        *ExtensionsLogs::Logs::log("info.n++") << QString("\t%1: %2 (total %3)")
                                                  .arg(iterator.key())
                                                  .arg(val.second * 1. / val.first)
                                                  .arg(val.first) << endl;
        qDebug().noquote() << QString("\t%1: %2 (total %3)")
                              .arg(iterator.key())
                              .arg(val.second * 1. / val.first)
                              .arg(val.first);
        ++iterator;
    }
    *ExtensionsLogs::Logs::log("info.n++") << QString("TOTAL: %1").arg(totalProjective * 1. / totalSent) << endl;
    qDebug().noquote() << QString("TOTAL: %1").arg(totalProjective * 1. / totalSent);
}

void SynTagRusParserTest::withoutSerialization()
{
    parsingTest();
    _syntagrusParser.getTreeCorpora()->size();
    _grammarParser.parseTree(*_syntagrusParser.getTreeCorpora());
    qDebug() << "Grammar size before serialization" << _grammarParser.getGrammar()->size();

    _syntagrusParser.serializeTreeCorpora();
    _grammarParser.serializeGrammar();

    testCYKSyntacticalAnalyzer();
}

void SynTagRusParserTest::wholeTest()
{
    parsingTest();
    treeCorporaSerializationTest();
    grammarTest();

    qDebug() << "Size of TreeCorpora after deserialization" << _syntagrusParser.getTreeCorpora()->size();
    qDebug() << "Grammar size after serialization" << _grammarParser.getGrammar()->size();

    testCYKSyntacticalAnalyzer();
}

void SynTagRusParserTest::doTest()
{
    static QDateTime startTime;
    parsingTest();
    treeCorporaSerializationTest();
    grammarTest();

    ExtensionsLogs::Logs::registerLogStream("info.n++");
    // area
    CNFGrammar *grammar=_grammarParser.getGrammar();
    grammar->calcSimpleRuleTrees();
//    QString result = grammar->toReport(_syntagrusParser.getTreeCorpora()->featureMapper());
//    qDebug().noquote() << result;
//    *ExtensionsLogs::Logs::log("info.n++") << result;
//    return;
    FeatureMapper fmapper;
    LinkMapper lmapper;
    fmapper.load();
    lmapper.load();
    CYKSyntacticalAnalyzer an(fmapper, lmapper);
    an.setMaxTime(60 * 1000);
//    return;
//    const int sz = 10;
    SynTagRusParser parser;
    parser.parse("../../rutextparserqt/test");
    QMultiHash<int, SentenceInCorpora> sentences(parser.getTreeCorpora()->sentencesBySize());
//    QMultiHash<int, SentenceInCorpora> sentences(parser.getTreeCorpora()->sentencesBySize());
    QMultiHash<int, SentenceInCorpora>::const_iterator it(sentences.constBegin());
    int /*first =0, second =0,*/ third =0, total=0;
    hack = grammar;
    fmapperHack = &fmapper;
    lmapperHack = &lmapper;
    while(it!=sentences.constEnd() /*&& it.key() == sz*/) {
        static int totalSentences = 0;
//        static int doneSentences = 0;
        SentenceInCorpora sentence = it.value();
        *ExtensionsLogs::Logs::log("info.n++") << tr("sentence (%1, %2) ").arg(sentence.getFilename()).arg(sentence.getId())
                 << sentence.qDebugSentence().wordList().join(',')
                 << '\n' << sentence.qDebugSentence().featList().join(',') << endl;
        if (!sentence.isProjective()) {
            qDebug() << "NOT PROJECTIVE";
            it++;
            continue;
        }
        if (sentence.size() < 15 || sentence.size() > 20) {
            it++;
            continue;
        }
        bool ok = true;
        foreach (const WordInCorpora &word, sentence.qDebugSentence().vector()) {
            if (!fmapper.isValid(fmapper.index(word.feature()))) {
                ok = false;
            }
        }
        if (!ok) {
            it++;
            continue;
        }
        ++totalSentences;
        startTime = QDateTime::currentDateTimeUtc();
        qDebug() << "starting analyzing";

        QList<QSharedPointer<RuleNode> > analyzedSentences = an.analyze(toAmbigious(sentence.qDebugSentence()), *_grammarParser.getGrammar());
        *ExtensionsLogs::Logs::log("info.n++") << "analyzedSz " << analyzedSentences.size() << endl;
        total += analyzedSentences.size();
        std::sort(analyzedSentences.begin(), analyzedSentences.end(), compareByProb);
        int found = 0;
        for (int i=0; i < analyzedSentences.size(); ++i) {
            QSharedPointer<RuleNode> rn = analyzedSentences.at(i);
//            bool founded = false;
            if (checkEqual(rn.data(), sentence.root(), _grammarParser.getGrammar()->rulesByID(), fmapper, lmapper)) {
                found++;
//                founded = true;
                *ExtensionsLogs::Logs::log("info.n++") << QString("Third Found at %1 of %2").arg(i).arg(analyzedSentences.size()) << endl;
                third+= i;
            }
//            logStream << QString("[%1](%2) ")
//                         .arg(i)
//                         .arg(rn->calcProb(-1, *grammar, fmapper, lmapper))
//                      << (founded ? "FOUNDED:\n" : "\n")<< QString(" of %1 ").arg(analyzedSentences.size())
//                      << rn->toString( _grammarParser.getGrammar()->rulesByID(), fmapper, lmapper)
//                      << endl;
        }

        static int totalFound = 0, actualFound = 0;
        ++totalFound;
        if (found)
            actualFound++;
        if (total)
        *ExtensionsLogs::Logs::log("info.n++") << QString("Current score [%1] (%2 [%3 %4])")
                                                  .arg(third * 1. / total)
                                                  .arg(found?"FOUND":"NOT FOUND")
                                                  .arg(actualFound).arg(totalFound)
                                               << endl;
        if (found)
            qDebug() << "NICE, FOUND" << found;
        else
            qDebug() << "SAD, NOT FOUND";
//        Q_ASSERT(found == 1);
//        QVERIFY2(!analyzedSentences.isEmpty(), "not found");
//        qDebug() << "analyzedSentences";
        //    foreach (const QStringList &sl, analyzedSentences)
        //        qDebug() << "\tSentence:" << sl;
        *ExtensionsLogs::Logs::log("info.n++") << QString("Time past %1").arg(
                                                      startTime.secsTo(QDateTime::currentDateTimeUtc()))
                                               << endl;
        it++;
    }
    qDebug() << "all done";
}


//QTEST_MAIN(SynTagRusParserTest)

int main(int /*argc*/, char **/*argv*/)
{
    SynTagRusParserTest tc;
//    tc.grammarTest();
    tc.testCYKSyntacticalAnalyzer();
//    tc.testLogic();
//    tc.testFirstGrammar();
//    tc.testFirstGrammar2();
//    tc.testSimpleFirst();
//    tc.testSecondGrammar();
//    tc.testSecondGrammar2();
    return 0;
//    QTEST_SET_MAIN_SOURCE_PATH
//    return QTest::qExec(&tc, argc, argv);
}

//#include "tst_all.moc"
