#include <QString>
#include <QtTest>

#include "syntagrusparser.h"

class SynTagRusParserTest : public QObject
{
    Q_OBJECT

    SynTagRusParser _parser;
public:
    SynTagRusParserTest();

private Q_SLOTS:
    void testCase1();

private:
    void parsingTest();
};

SynTagRusParserTest::SynTagRusParserTest()
{
}

void SynTagRusParserTest::parsingTest()
{
    _parser.getGrammar()->clear();

    _parser.parse();

    QVERIFY2(!_parser.getGrammar()->isEmpty(), "CNFGrammar is empty after parsing");
    qDebug() << _parser.getGrammar()->size();
}

void SynTagRusParserTest::testCase1()
{
//    QBENCHMARK {
        parsingTest();
//    }
}

QTEST_APPLESS_MAIN(SynTagRusParserTest)

#include "tst_syntagrusparsertest.moc"
