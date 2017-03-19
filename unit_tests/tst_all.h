#ifndef TST_ALL_H
#define TST_ALL_H

#include <QString>
#include <QtTest>

#include "corpora_parsing/syntagrusparser.h"
#include "corpora_parsing/optimizedtreecorpora.h"
#include "treeparser.h"
#include "cyksyntacticalanalyzer.h"
#include "morphclient.h"

class SynTagRusParserTest : public QObject
{
    Q_OBJECT

    SynTagRusParser _syntagrusParser;
    TreeParser _grammarParser;
    MorphClient _client;
    OptimizedTreeCorpora _optimized;

    QFile logFile;
    QFile extraLogFile;
    QTextStream logStream;
    QTextStream extraLogStream;
public:
    SynTagRusParserTest() : logFile( "logs_test.n++") , extraLogFile( "logs_info_test.n++"){
        if (!logFile.open(QIODevice::WriteOnly))
            Q_ASSERT(false);

        if (!extraLogFile.open(QIODevice::WriteOnly))
            Q_ASSERT(false);

        logStream.setDevice(&logFile);
        extraLogStream.setDevice(&extraLogFile);

        _client.connectToHost(QHostAddress(QHostAddress::LocalHost).toString(), 14147);
    }

private:
    bool morphClientTestSentence(const OptimizedSentence &sentence, const int &inn);
    bool morphClientTestWord(const OptimizedWord &word, const int &inn);

private:
    void treeCorporaSerializationTest();
    void testCYKSyntacticalAnalyzer();


    void wholeTest();
    void withoutSerialization();
    void test();

    void parsingTest();
    void dawgTest();
    void dawgLoadTest();

    void simpleTestMorphClient();
    void testMorphClient();
private Q_SLOTS:
    void grammarTest();
};

#endif // TST_ALL_H
