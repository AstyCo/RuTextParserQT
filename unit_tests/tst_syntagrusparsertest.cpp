#include <QString>
#include <QtTest>

#include "corpora_parsing/syntagrusparser.h"
#include "treeparser.h"
#include "cyksyntacticalanalyzer.h"

class SynTagRusParserTest : public QObject
{
    Q_OBJECT

    SynTagRusParser _syntagrusParser;
    TreeParser _grammarParser;
public:
    SynTagRusParserTest() { }

private:
    void parsingTest();
    void treeCorporaSerializationTest();
    void grammarSerializationTest();
    void testCYKSyntacticalAnalyzer();


    void wholeTest();
    void test();
private Q_SLOTS:
    void withoutSerialization();

};

void SynTagRusParserTest::test()
{
//    QBENCHMARK {

    _syntagrusParser.deserializeTreeCorpora();
    qDebug() << "Size of TreeCorpora after deserialization" << _syntagrusParser.getTreeCorpora()->size();
    _grammarParser.deserializeGrammar();
    qDebug() << "Grammar size after serialization" << _grammarParser.getGrammar()->size();

    testCYKSyntacticalAnalyzer();

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

void SynTagRusParserTest::grammarSerializationTest()
{
    _grammarParser.parseTree(*_syntagrusParser.getTreeCorpora());

    int sz = _grammarParser.getGrammar()->size();
    qDebug() << "Grammar size before serialization" << sz;

    _grammarParser.serializeGrammar();
    _grammarParser.deserializeGrammar();

    qDebug() << "Grammar size after serialization" << _grammarParser.getGrammar()->size();
    QVERIFY2(sz == _grammarParser.getGrammar()->size(),
             "Grammar of different size after serialization/deserialization");
}

AmbigiousStringVector toAmbigious(const QStringList &sentence) {
    AmbigiousStringVector res;

    foreach (const QString &word, sentence){
        QList<QStringList> wlist;
        wlist.append(QStringList(word));
        res.append(wlist);
    }
    return res;
}

void SynTagRusParserTest::testCYKSyntacticalAnalyzer()
{
    SentenceInCorpora sentence = _syntagrusParser.getTreeCorpora()->at(0).at(4);
    Q_ASSERT(!sentence.skip());
    qDebug() << "sentence" << sentence.qDebugSentence();

    qDebug() << "starting analyzing";

    Sentences analyzedSentences = CYKSyntacticalAnalyzer::analyze(toAmbigious(sentence.qDebugSentence()), *_grammarParser.getGrammar());

    QVERIFY2(!analyzedSentences.isEmpty(), "not found");
    qDebug() << "analyzedSentences";
    foreach (const QStringList &sl, analyzedSentences)
        qDebug() << "\tSentence:" << sl;
}

void SynTagRusParserTest::withoutSerialization()
{
    parsingTest();
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
    grammarSerializationTest();

    qDebug() << "Size of TreeCorpora after deserialization" << _syntagrusParser.getTreeCorpora()->size();
    qDebug() << "Grammar size after serialization" << _grammarParser.getGrammar()->size();

    testCYKSyntacticalAnalyzer();
}



QTEST_APPLESS_MAIN(SynTagRusParserTest)

#include "tst_syntagrusparsertest.moc"
