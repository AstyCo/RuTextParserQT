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

bool SynTagRusParserTest::morphClientTestSentence(const OptimizedSentence &opt, const int &inn)
{
    static long failedCount = 0;
    static long suc = 0;
    bool failed = false;
    for (int i=0; i<opt.words().size(); ++i) {
        const OptimizedWord &word = opt.words().at(i);
        if (!morphClientTestWord(word, inn)) {
            failed = true;
        }
    }
    if (failed) {
        failedCount++;
        logStream << QString("[%1/%2]: ")
                     .arg(QString::number(suc))
                     .arg(QString::number(failedCount))
                 << "failed sentence\n\t"
                 << opt.getSentence(inn).join(" ")
                 << endl;
    }
    else {
        suc++;
        extraLogStream << QString("[%1/%2]: ")
                          .arg(QString::number(suc))
                          .arg(QString::number(failedCount))
                 << "success sentence\n\t"
                 << opt.getSentence(inn).join(" ")
                 << endl;
    }

    return !failed;
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

bool SynTagRusParserTest::morphClientTestWord(const OptimizedWord &word, const int &inn)
{
    static long failedCount = 0;
    static long successfulCount = 0;
    bool successful = false;

    QString feature = _optimized.featureMapper().value(word.feature);
    MorphResultType analyzed = _client.analyzeWord(word.words[inn]);

    for (int i=0; i < analyzed.size(); ++i) {
        if (analyzed.at(i).first == feature)
            // found
            successful = true;
    }

    if (!successful) {
        ++failedCount;
//        Q_ASSERT(failedCount < 100);
        logStream << QString("[%1/%2]:")
                    .arg(QString::number(successfulCount))
                    .arg(QString::number(failedCount))
                 << " failed word\n\t"
                 << word.words.at(inn) << ' ' << feature
                 << endl;
        logStream << toString(analyzed)
                  << endl;
    }
    else {
        successfulCount++;
//        Q_ASSERT(successfulCount < 1000);
        extraLogStream <<  QString("[%1/%2]:")
                        .arg(QString::number(successfulCount))
                        .arg(QString::number(failedCount))
                     << " success word\n\t"
                     << word.words.at(inn) << ' ' << feature
                     << endl;
        extraLogStream << toString(analyzed)
                  << endl;
    }

    return successful;
}

void SynTagRusParserTest::treeCorporaSerializationTest()
{
    TreeCorpora *treeCorpora = _syntagrusParser.getTreeCorpora();
    long long sizeBefore = treeCorpora->size();

    _syntagrusParser.serializeTreeCorpora();
    _syntagrusParser.deserializeTreeCorpora();

    qDebug() << "size after deserialization" << _syntagrusParser.getTreeCorpora()->size();

     QVERIFY2(sizeBefore == _syntagrusParser.getTreeCorpora()->size(),
              "TreeCorpora of different size after serialization/deserialization");
}

void SynTagRusParserTest::grammarTest()
{
    _syntagrusParser.deserializeTreeCorpora();
    _grammarParser.parseTree(*_syntagrusParser.getTreeCorpora());

    int sz = _grammarParser.getGrammar()->size();
    qDebug() << "Grammar size before serialization" << sz;

    _grammarParser.serializeGrammar();
    _grammarParser.deserializeGrammar();

    qDebug() << "Grammar size after deserialization" << _grammarParser.getGrammar()->size();
    QVERIFY2(sz == _grammarParser.getGrammar()->size(),
             "Grammar of different size after serialization/deserialization");
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



void SynTagRusParserTest::testCYKSyntacticalAnalyzer()
{
    _syntagrusParser.deserializeTreeCorpora();
    _grammarParser.deserializeGrammar();
    CYKSyntacticalAnalyzer an(_syntagrusParser.getTreeCorpora()->featureMapper(),
                              _syntagrusParser.getTreeCorpora()->linkMapper());

    SentenceInCorpora sentence = _syntagrusParser.getTreeCorpora()->sentencesBySize().find(20).value();
    Q_ASSERT(!sentence.skip());
    qDebug() << "sentence" << sentence.qDebugSentence().wordList()
             << '\n' << sentence.qDebugSentence().featList();

    qDebug() << "starting analyzing";

    QList<QSharedPointer<RuleNode> > analyzedSentences = an.analyze(toAmbigious(sentence.qDebugSentence()), *_grammarParser.getGrammar());

    QVERIFY2(!analyzedSentences.isEmpty(), "not found");
    qDebug() << "analyzedSentences";
    qDebug() << "analyzedSz" << analyzedSentences.size();
//    foreach (const QStringList &sl, analyzedSentences)
//        qDebug() << "\tSentence:" << sl;
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



QTEST_APPLESS_MAIN(SynTagRusParserTest)

//#include "tst_all.moc"
