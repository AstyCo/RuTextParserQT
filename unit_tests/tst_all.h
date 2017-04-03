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
    enum MorphStates
    {
        failedState,
        successedState,
        skippedState
    };

    MorphStates morphClientTestSentence(const OptimizedSentence &sentence, const int &inn);
    MorphStates morphClientTestWord(const OptimizedWord &word, const int &inn);
    bool checkEqual(RuleNode *ruleNode, RecordNode *recordNode,
                    const QVector<ScoredChomskyRuleRecord> &ruleByID,
                    const FeatureMapper &fmapper, const LinkMapper &lmapper);

private:
    void deserializeTreeCorpora();
    void deserializeGrammar();

    void treeCorporaSerializationTest();


    void withoutSerialization();
    void test();
    void simpleTestMorphClient();

    void parsingTest();
    void dawgTest();
    void dawgLoadTest();

    void grammarTest();

    void testMorphClient();
    void testCYKSyntacticalAnalyzer();
private Q_SLOTS:
    void wholeTest();
};

#endif // TST_ALL_H
