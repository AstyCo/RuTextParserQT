#include "treeparser.h"
#include "grammar/rulerecord.h"

#include "rutextparser_extensions.h"

TreeParser::TreeParser()
    : _grammar(NULL)
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

//void TreeParser::parseTree(const OptimizedTreeCorpora &treeCorpora)
//{
////    foreach (const TextInCorpora & text, treeCorpora)
////        foreach (const SentenceInCorpora &sentence, text) {
////            parseSentence(sentence);
////        }
//    foreach(const OptimizedSentence &sentence, treeCorpora.multihashSentences())
//        parseSentence(sentence);
//}

void TreeParser::parseTree(const TreeCorpora &treeCorpora)
{
    if (_grammar)
        _grammar->clear();
    else
        _grammar = new CNFGrammar(treeCorpora.featureMapper().size());

//    // starts building
//    _grammar->startBuilding();
    // building
    foreach(const SentenceInCorpora &sentence, treeCorpora.sentencesBySize())
        parseSentence(sentence,
                      treeCorpora.featureMapper(),
                      treeCorpora.linkMapper());
//    // finish building
//    _grammar->finishBuilding();
}

CNFGrammar *TreeParser::getGrammar()
{
    return _grammar;
}

const CNFGrammar *TreeParser::getGrammar() const
{
    return _grammar;
}

#include <QDebug>

void TreeParser::serializeGrammar() const
{
    if (!_grammar) {
        printWarning("TreeParser::serializeGrammar: _grammar is null");
        return;
    }
    ExtensionsSerialization::dumpToFile(_dumpFilenameGrammar, *_grammar);

    qDebug() << "Grammar serialized";
}

void TreeParser::deserializeGrammar()
{
    if (_grammar)
        _grammar->clear();
    else
        _grammar = new CNFGrammar();

    ExtensionsSerialization::loadFromDump(_dumpFilenameGrammar, *_grammar);
}

void TreeParser::parseSentence(const SentenceInCorpora &sentence,
                               const UniqueVector<featureID, QString> &fmapper,
                               const UniqueVector<linkID, QString> &lmapper)
{
//    qDebug() << "parse" << sentence.qDebugSentence().wordList();
    parseNode(sentence.root(), fmapper, lmapper);
}

void TreeParser::parseNode(const RecordNode *node,
                           const UniqueVector<featureID, QString> &fmapper,
                           const UniqueVector<linkID, QString> &lmapper)
{
    if (!node) {
        printWarning("TreeParser::parseNode: null node");
        return;
    }

    _grammar->append(fmapper, lmapper, node, -1);
}


#include <QDebug>
