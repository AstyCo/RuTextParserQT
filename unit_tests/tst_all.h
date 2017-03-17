#ifndef TST_ALL_H
#define TST_ALL_H

#include <QString>
#include <QtTest>

#include "corpora_parsing/syntagrusparser.h"
#include "corpora_parsing/optimizedtreecorpora.h"
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
    void treeCorporaSerializationTest();
    void grammarSerializationTest();
    void testCYKSyntacticalAnalyzer();


    void wholeTest();
    void withoutSerialization();
    void test();

    void parsingTest();
    void dawgTest();
    void dawgLoadTest();
private Q_SLOTS:
    void testMorphClient();
};

#endif // TST_ALL_H
