#include "tst_all.h"

#include "dawgdic/dawg-builder.h"
#include "dawgdic/dictionary-builder.h"
#include "morphclient.h"



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

SynTagRusParserTest::MorphStates SynTagRusParserTest::morphClientTestSentence(const OptimizedSentence &opt, const int &inn)
{
    static long failedCount = 0;
    static long skippedCount = 0;
    static long suc = 0;
    bool failed = false;
    bool skipped = false;
    for (int i=0; i<opt.words().size(); ++i) {
        const OptimizedWord &word = opt.words().at(i);
        switch (morphClientTestWord(word, inn)) {
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
        logStream << QString("[%1/%2/%3]: ")
                     .arg(QString::number(suc))
                     .arg(skippedCount)
                     .arg(QString::number(failedCount))
                 << "failed sentence\n\t"
                 << opt.getSentence(inn).join(" ")
                 << endl;
    }
    else if (failed) {
        failedCount++;
        logStream << QString("[%1/%2/%3]: ")
                     .arg(QString::number(suc))
                     .arg(skippedCount)
                     .arg(QString::number(failedCount))
                 << "failed sentence\n\t"
                 << opt.getSentence(inn).join(" ")
                 << endl;
    }
    else {
        suc++;
        extraLogStream << QString("[%1/%2/%3]: ")
                          .arg(QString::number(suc))
                          .arg(skippedCount)
                          .arg(QString::number(failedCount))
                 << "success sentence\n\t"
                 << opt.getSentence(inn).join(" ")
                 << endl;
    }

    return (skipped ? skippedState : (failed ? failedState : successedState));
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
    static long failedCount = 0;
    static long skippedCount = 0;
    static long successfulCount = 0;
    bool successful = false;

    QString feature = _optimized.featureMapper().value(word.feature);
    MorphResultType analyzed = _client.analyzeWord(word.words[inn]);

    for (int i=0; i < analyzed.size(); ++i) {
        if (analyzed.at(i).first == feature)
            // found
            successful = true;
    }

    QString what;
    if (!successful) {
        if (analyzed.isEmpty()) {
            ++skippedCount;
            what = "skipped";
        }
        else {
            ++failedCount;
            what = "failed";
        }
//        Q_ASSERT(failedCount < 100);
        logStream << QString("[%1/%2/%3]:")
                    .arg(QString::number(successfulCount))
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
//        Q_ASSERT(successfulCount < 1000);
        extraLogStream <<  QString("[%1/%2/%3]:")
                           .arg(QString::number(successfulCount))
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
    return successedState;
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
    return lhs->calcProb2(-1, *hack, *fmapperHack, *lmapperHack, 1)
            > rhs->calcProb2(-1, *hack, *fmapperHack, *lmapperHack, 1);
}
bool compareByProb22(QSharedPointer<RuleNode> lhs, QSharedPointer<RuleNode> &rhs)
{
    return lhs->calcProb2(-1, *hack, *fmapperHack, *lmapperHack, 2)
            > rhs->calcProb2(-1, *hack, *fmapperHack, *lmapperHack, 2);
}





void SynTagRusParserTest::testCYKSyntacticalAnalyzer()
{
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
//    return;
    const int sz = 10;
    QMultiHash<int, SentenceInCorpora> sentences(_syntagrusParser.getTreeCorpora()->sentencesBySize());
//    QMultiHash<int, SentenceInCorpora> sentences(parser.getTreeCorpora()->sentencesBySize());
    QMultiHash<int, SentenceInCorpora>::const_iterator it(sentences.constFind(sz));
    int first =0, second =0, third =0, total=0;
    hack = grammar;
    fmapperHack = &fmapper;
    lmapperHack = &lmapper;
    while(it!=sentences.constEnd() && it.key() == sz) {
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

        QList<QSharedPointer<RuleNode> > analyzedSentences = an.analyze(toAmbigious(sentence.qDebugSentence()), *_grammarParser.getGrammar());
        qDebug() << "analyzedSz" << analyzedSentences.size();
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
                }
                logStream << QString("[%1](%2) ")
                             .arg(i)
                             .arg(rn->calcProb2(-1, *grammar, fmapper, lmapper, 1))
                          << (founded ? "FOUNDED:\n" : "\n") << QString(" of %1 ").arg(analyzedSentences21.size())
                          << rn->toString( _grammarParser.getGrammar()->rulesByID(), fmapper, lmapper)
                          << endl;
            }
            if (found)
                qDebug() << "NICE, FOUND" << found;
            else
                qDebug() << "SAD, NOT FOUND";
            Q_ASSERT(found == 1);
            QVERIFY2(!analyzedSentences21.isEmpty(), "not found");
        }
        {
            QList<QSharedPointer<RuleNode> > analyzedSentences22 = analyzedSentences;
            std::sort(analyzedSentences22.begin(), analyzedSentences22.end(), compareByProb22);
            int found = 0;
            for (int i=0; i < analyzedSentences22.size(); ++i) {
                QSharedPointer<RuleNode> rn = analyzedSentences22.at(i);
                bool founded = false;
                if (checkEqual(rn.data(), sentence.root(), _grammarParser.getGrammar()->rulesByID(), fmapper, lmapper)) {
                    found++;
                    founded = true;
                    *ExtensionsLogs::Logs::log("info.n++") << QString("Second Found at %1 of %2").arg(i).arg(analyzedSentences22.size()) << endl;
                    second+= i;
                }
                logStream << QString("[%1](%2) ")
                             .arg(i)
                             .arg(rn->calcProb2(-1, *grammar, fmapper, lmapper, 2))
                          << (founded ? "FOUNDED:\n" : "\n") << QString(" of %1 ").arg(analyzedSentences22.size())
                          << rn->toString( _grammarParser.getGrammar()->rulesByID(), fmapper, lmapper)
                          << endl;
            }
            if (found)
                qDebug() << "NICE, FOUND" << found;
            else
                qDebug() << "SAD, NOT FOUND";
            Q_ASSERT(found == 1);
            QVERIFY2(!analyzedSentences22.isEmpty(), "not found");
        }




        std::sort(analyzedSentences.begin(), analyzedSentences.end(), compareByProb);
        int found = 0;
        for (int i=0; i < analyzedSentences.size(); ++i) {
            QSharedPointer<RuleNode> rn = analyzedSentences.at(i);
            bool founded = false;
            if (checkEqual(rn.data(), sentence.root(), _grammarParser.getGrammar()->rulesByID(), fmapper, lmapper)) {
                found++;
                founded = true;
                *ExtensionsLogs::Logs::log("info.n++") << QString("Third Found at %1 of %2").arg(i).arg(analyzedSentences.size()) << endl;
                third+= i;
            }
            logStream << QString("[%1](%2) ")
                         .arg(i)
                         .arg(rn->calcProb(-1, *grammar, fmapper, lmapper))
                      << (founded ? "FOUNDED:\n" : "\n")<< QString(" of %1 ").arg(analyzedSentences.size())
                      << rn->toString( _grammarParser.getGrammar()->rulesByID(), fmapper, lmapper)
                      << endl;
        }

        *ExtensionsLogs::Logs::log("info.n++") << QString("Current scores [%1][%2][%3]")
                                                  .arg(first * 1. / total)
                                                  .arg(second * 1. / total)
                                                  .arg(third * 1. / total) << endl;

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
//    return;
//    const int sz = 10;
    SynTagRusParser parser;
    parser.parse("../../rutextparserqt/test");
    QMultiHash<int, SentenceInCorpora> sentences(parser.getTreeCorpora()->sentencesBySize());
//    QMultiHash<int, SentenceInCorpora> sentences(parser.getTreeCorpora()->sentencesBySize());
    QMultiHash<int, SentenceInCorpora>::const_iterator it(sentences.constBegin());
    int first =0, second =0, third =0, total=0;
    hack = grammar;
    fmapperHack = &fmapper;
    lmapperHack = &lmapper;
    while(it!=sentences.constEnd() /*&& it.key() == sz*/) {
        SentenceInCorpora sentence = it.value();
        *ExtensionsLogs::Logs::log("info.n++") << tr("sentence (%1, %2) ").arg(sentence.getFilename()).arg(sentence.getId())
                 << sentence.qDebugSentence().wordList().join(' ')
                 << '\n' << sentence.qDebugSentence().featList().join(' ') << endl;
        if (!sentence.isProjective()) {
            qDebug() << "NOT PROJECTIVE";
            it++;
            continue;
        }
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



int main(int /*argc*/, char **/*argv[]*/)
{
    SynTagRusParserTest tc;
    tc.doTest();
    return 0;
//    QTEST_SET_MAIN_SOURCE_PATH
//    return QTest::qExec(&tc, argc, argv);
}

//#include "tst_all.moc"
