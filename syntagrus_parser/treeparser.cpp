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
//    foreach (const TextInCorpora & text, treeCorpora)
//        foreach (const SentenceInCorpora &sentence, text) {
//            parseSentence(sentence);
//        }
    foreach(const SentenceInCorpora &sentence, treeCorpora.sentencesBySize())
        parseSentence(sentence);
}

CNFGrammar *TreeParser::getGrammar()
{
    return &_grammar;
}

const CNFGrammar *TreeParser::getGrammar() const
{
    return &_grammar;
}

#include <QDebug>

void TreeParser::serializeGrammar() const
{
    ExtensionsSerialization::dumpToFile(_dumpFilenameGrammar, _grammar);

    qDebug() << "Grammar serialized";
}

void TreeParser::deserializeGrammar()
{
    _grammar.clear();

    ExtensionsSerialization::loadFromDump(_dumpFilenameGrammar, _grammar);
}

void TreeParser::parseSentence(const SentenceInCorpora &sentence)
{
    parseNode(_ROOT, sentence.root());
}

void TreeParser::parseNode(const Nonterminal &leftNonterminal, const RecordNode *node)
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
                Nonterminal newFirst = generateUniqueNonterminalName();
                Nonterminal newSecond = generateUniqueNonterminalName();

                _grammar.append(new RuleCNFGrammar(leftNonterminal, newFirst, newSecond));

                produceNonterminalRules(newFirst, leftRules);
                produceDotNonterminalRule(newSecond, node, rightRules);
            }
        }
    }
}

void TreeParser::produceTerminalRule(const Nonterminal &leftNonterminal, const RecordNode *node)
{
    _grammar.append(new RuleCNFGrammar(leftNonterminal, node->record()._feat));
}

void TreeParser::produceNonterminalRules(Nonterminal leftNonterminal, QList<RecordNode *> &nodes)
{
    Q_ASSERT(!nodes.isEmpty());

    for (int i = 0; i < nodes.size() - 1; ++i) {
        Nonterminal newFirst = generateUniqueNonterminalName();
        Nonterminal newSecond = generateUniqueNonterminalName();

        _grammar.append(new RuleCNFGrammar(leftNonterminal, newFirst, newSecond));
        parseNode(newFirst, nodes[i]);

        leftNonterminal = newSecond;
    }

    parseNode(leftNonterminal, nodes.last());
}

void TreeParser::produceDotNonterminalRule(const Nonterminal &leftNonterminal,
                                           const RecordNode *node,
                                           QList<RecordNode *> &nodes, bool dotThenRule)
{
    Nonterminal newDotRule = generateUniqueNonterminalName();
    Nonterminal newDotRuleSecond = generateUniqueNonterminalName();

    if(dotThenRule)
        _grammar.append(new RuleCNFGrammar(leftNonterminal, newDotRule, newDotRuleSecond));
    else
        _grammar.append(new RuleCNFGrammar(leftNonterminal, newDotRuleSecond, newDotRule));

    produceTerminalRule(newDotRule, node);
    produceNonterminalRules(newDotRuleSecond, nodes);
}

#include <QDebug>

Nonterminal TreeParser::generateUniqueNonterminalName() const
{
    static Nonterminal n = FIRST_EMPTY_NONTERMINAL;

    return n++;
}
