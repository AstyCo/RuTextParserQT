#include <QString>
#include <QtTest>

#include "corpora_parsing/syntagrusparser.h"
#include "treeparser.h"

class SynTagRusParserTest : public QObject
{
    Q_OBJECT

    SynTagRusParser _syntagrusParser;
    TreeParser _grammarParser;
public:
    SynTagRusParserTest();

private:
    void parsingTest();
    void treeCorporaSerializationTest();
    void grammarSerializationTest();

private Q_SLOTS:
    void test();

};

SynTagRusParserTest::SynTagRusParserTest()
{
}

void SynTagRusParserTest::parsingTest()
{
    _syntagrusParser.parse();

    // test TreeCorpora
    {
        QVERIFY2(!_syntagrusParser.getTreeCorpora()->isEmpty(), "TreeCorpora is empty after parsing");
        qDebug() << _syntagrusParser.getTreeCorpora()->size();
    }

//    QVERIFY2(!_parser.getGrammar()->isEmpty(), "CNFGrammar is empty after parsing");
    //    qDebug() << _parser.getGrammar()->size();
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

void SynTagRusParserTest::test()
{
    QBENCHMARK {
//        parsingTest();

    _syntagrusParser.deserializeTreeCorpora();
    qDebug() << "Size of TreeCorpora after deserialization" << _syntagrusParser.getTreeCorpora()->size();
    _grammarParser.deserializeGrammar();
    qDebug() << "Grammar size after serialization" << _grammarParser.getGrammar()->size();

    }
}


QTEST_APPLESS_MAIN(SynTagRusParserTest)

#include "tst_syntagrusparsertest.moc"
