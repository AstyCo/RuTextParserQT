#include "treeparser.h"

#include "rutextparser_extensions.h"

TreeParser::TreeParser()
{
    static QString dumpFilenameGrammar("dumps/cnfgrammar.dump");

#ifdef RUTEXTPARSERQT_DIR
    _dumpFilenameGrammar = RUTEXTPARSERQT_DIR + dumpFilenameGrammar;
#else
    qWarning("RUTEXTPARSERQT_DIR not defined");

    QString manualPath = "../../RuTextParserQT/";
    _dumpFilenameGrammar = manualPath + dumpFilenameGrammar;
#endif
}

void TreeParser::parseTree(const TreeCorpora &treeCorpora)
{
    foreach (const TextInCorpora & text, treeCorpora)
        foreach (const SentenceInCorpora &sentence, text) {
            parseSentence(sentence);
        }
}

CNFGrammar *TreeParser::getGrammar()
{
    return &_grammar;
}

const CNFGrammar *TreeParser::getGrammar() const
{
    return &_grammar;
}

void TreeParser::serializeGrammar() const
{
    ExtensionsSerialization::dumpToFile(_dumpFilenameGrammar, _grammar);
}

void TreeParser::deserializeGrammar()
{
    _grammar.clear();

    ExtensionsSerialization::loadFromDump(_dumpFilenameGrammar, _grammar);
}

void TreeParser::parseSentence(const SentenceInCorpora &sentence)
{
    parseNode("_root", sentence.root());
}

void TreeParser::parseNode(const QString &leftNonterminal, const RecordNode *node)
{
    const QList<RecordNode *> &childNodes = node->childNodes();

    if (childNodes.isEmpty()) {
        produceTerminalRule(leftNonterminal, node);
    }
    else {
        QList<RecordNode *> leftRules;
        QList<RecordNode *> rightRules;

        // separate by order: going before or after?
        foreach (RecordNode *rnode, childNodes) {
            if (rnode->record().before(node->record()))
                leftRules.append(rnode);
            else
                rightRules.append(rnode);
        }

        if (leftRules.isEmpty()) {
            if (rightRules.isEmpty()) {
                Q_ASSERT(false);
                return;
            }
            produceDotNonterminalRule(leftNonterminal, node, rightRules);
        }
        else {
            if (rightRules.isEmpty()) {
                produceDotNonterminalRule(leftNonterminal, node, leftRules, false);
            }
            else {
                QString newFirst = generateUniqueNonterminalName();
                QString newSecond = generateUniqueNonterminalName();

                _grammar.append(RuleCNFGrammar(leftNonterminal, newFirst, newSecond));

                produceNonterminalRules(newFirst, leftRules);
                produceDotNonterminalRule(newSecond, node, rightRules);
            }
        }
    }
}

void TreeParser::produceTerminalRule(const QString &leftNonterminal, const RecordNode *node)
{
    _grammar.append(RuleCNFGrammar(leftNonterminal, node->record()._feat));
}

void TreeParser::produceNonterminalRules(QString leftNonterminal, QList<RecordNode *> &nodes)
{
    Q_ASSERT(!nodes.isEmpty());

    for (int i = 0; i < nodes.size() - 1; ++i) {
        QString newFirst = generateUniqueNonterminalName();
        QString newSecond = generateUniqueNonterminalName();

        _grammar.append(RuleCNFGrammar(leftNonterminal, newFirst, newSecond));
        parseNode(newFirst, nodes[i]);

        leftNonterminal = newSecond;
    }

    parseNode(leftNonterminal, nodes.last());
}

void TreeParser::produceDotNonterminalRule(const QString &leftNonterminal,
                                           const RecordNode *node,
                                           QList<RecordNode *> &nodes, bool dotThenRule)
{
    QString newDotRule = generateUniqueNonterminalName();
    QString newDotRuleSecond = generateUniqueNonterminalName();

    if(dotThenRule)
        _grammar.append(RuleCNFGrammar(leftNonterminal, newDotRule, newDotRuleSecond));
    else
        _grammar.append(RuleCNFGrammar(leftNonterminal, newDotRuleSecond, newDotRule));

    produceTerminalRule(newDotRule, node);
    produceNonterminalRules(newDotRuleSecond, nodes);
}

QString TreeParser::generateUniqueNonterminalName() const
{
    static long long n = 0;

    return "_" + QString::number(n++);
}
