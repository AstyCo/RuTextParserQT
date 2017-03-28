#include "cnfgrammar.h"

#include "rutextparser_extensions.h"

#include <QFile>
#include <QStringList>

#include <QDebug>


CNFGrammar::CNFGrammar()
    : _size(0)
{
    initDumpFilename();
}

CNFGrammar::CNFGrammar(const featureID &featureCount)
    : _size(featureCount)
{
    initDumpFilename();

    resizeVectors(featureCount);
}

CNFGrammar::~CNFGrammar()
{
    clear();
}

//void CNFGrammar::finishBuilding()
//{
//    Q_ASSERT(_plistRulesByFID != NULL);

//    for (int i=0; i<_size; ++i)
//        _rulesByFeatureID[i] = listScoredToSetScored(_plistRulesByFID->at(i));

//    delete [] _plistRulesByFID;
//    _plistRulesByFID = NULL;
//}

void CNFGrammar::append(const RuleRecord &rule)
{
    ListRuleID ruleIDs;
    foreach (const RuleDepend &depend, rule.depends()) {
        ruleIDs.append( insert(
                    ChomskyRuleRecord(depend.link, depend.terminal,
                                      rule.sourceRule(), depend.isRight)));
    }
//    std::sort(ruleIDs.begin(), ruleIDs.end());

    ListRuleID cykFormed = produceSequenceForCYK(ruleIDs);
    insert(rule.sourceRule(), cykFormed);
}

void CNFGrammar::addRoot(const featureID &fid)
{
    _rootScore[fid].increaseScore();
}



void CNFGrammar::clear()
{
    clearCache();

   _rootScore.clear();
    _ruleByID.clear();
    _rulesByFeatureID.clear();
    _rulesByFeatureIDReverse.clear();
}

void CNFGrammar::setDumpFilename(const QString &path)
{
    _dumpFilename = path;
}

void CNFGrammar::dump() const
{
    ExtensionsSerialization::dumpToFile(_dumpFilename, *this);
}

void CNFGrammar::loadFromDump()
{
    ExtensionsSerialization::loadFromDump(_dumpFilename, *this);
}

void CNFGrammar::resizeVectors(int sz)
{
    // init vectors
    _rootScore.resize(sz);
    _rulesByFeatureID.resize(sz);
    _rulesByFeatureIDReverse.resize(sz);

    resizeMatrix(sz);
}

void CNFGrammar::resizeMatrix(int sz)
{
    // init matrix
    _rulesByRightIDsHash.resize(sz);
    for (int i=0; i < sz; ++i)
        _rulesByRightIDsHash[i].resize(sz);
}

void CNFGrammar::initDumpFilename()
{
    static QString dumpFilename("dumps/cnfgrammar.dump");

#ifdef RUTEXTPARSERQT_DIR
    _dumpFilename = RUTEXTPARSERQT_DIR + dumpFilename;
#else
    qWarning("RUTEXTPARSERQT_DIR not defined");

    QString manualPath = "../../RuTextParserQT/";
    _dumpFilename = manualPath + dumpFilename;
#endif
}

void CNFGrammar::clearCache()
{
    _rulesByRightIDsHash.clear();
}

void CNFGrammar::fillCache(const int &newID)
{
    const ChomskyRuleRecord &rule = _ruleByID[newID].rule;

    _rulesByRightIDsHash[rule.leftID()][rule.rightID()].append(newID);
}

ruleID CNFGrammar::insert(const ChomskyRuleRecord &rule)
{
    ListRuleID::const_iterator i (find(rule));
    if (i != _rulesByRightIDsHash[rule.leftID()][rule.rightID()].constEnd()) {
        _ruleByID[*i].increaseScore();
        return *i;
    }

    ruleID newID = _ruleByID.size();
    // insert new rule
    _ruleByID.append(ScoredChomskyRuleRecord(rule));
    // fill cache
    fillCache(newID);
    return newID;
}

ListRuleID::const_iterator CNFGrammar::find(const ChomskyRuleRecord &rule) const
{
    const ListRuleID &rules = _rulesByRightIDsHash[rule.leftID()][rule.rightID()];
    ListRuleID::const_iterator i(rules.constBegin());
    while (i!=rules.constEnd()) {
        if (_ruleByID[*i].rule == rule)
            return i;
        ++i;
    }

    return rules.constEnd();
}

void CNFGrammar::insert(const featureID &srcRuleID, const ListRuleID &ids)
{
    if (ids.isEmpty())
        Q_ASSERT(false);
    // Simple insert
    SimpleRuleNode *node = _rulesByFeatureID[srcRuleID].node(ids);
    if (node) {
        // increase score of existing rule
        node->increaseScore();
    }
    else {
        // insert new rule
        _rulesByFeatureID[srcRuleID].insert(ids);
    }

//    // fill rules STRAIGHT
//    ListRuleID sorted(ids);
//    {
//        std::sort(sorted.begin(), sorted.end());
//        SimpleRuleNode *node = _rulesByFeatureID[srcRuleID].node(sorted);
//        if (node) {
//            // increase score of existing rule
//            node->increaseScore();
//        }
//        else {
//            // insert new rule
//            _rulesByFeatureID[srcRuleID].insert(sorted);
//        }
//    }
//    // fill rules BACKWARD
//    {
//        std::reverse(sorted.begin(), sorted.end());
//        SimpleRuleNode *node = _rulesByFeatureIDReverse[srcRuleID].node(sorted);
//        if (node) {
//            // increase score of existing rule
//            node->increaseScore();
//        }
//        else {
//            // insert new rule
//            _rulesByFeatureIDReverse[srcRuleID].insert(sorted);
//        }
    //    }
}

ListRuleID CNFGrammar::produceSequenceForCYK(const ListRuleID &ids) const
{
    ListRuleID result;
    // find first right rule
    int i = 0;
    for (;i<ids.size(); ++i) {
        if (_ruleByID[ids.at(i)].rule._isRightRule)
            break;
    }
    if (i < ids.size()) {
        // add right rules
        for (int j=i;j<ids.size(); ++j)
            result.append(ids.at(j));
    }
    if (i > 0) {
        // add left rules (reverted)
        for (int j=i-1; j >= 0; --j)
            result.append(ids.at(j));
    }
    return result;
}

ListRuleID CNFGrammar::fromCYK(const ListRuleID &ids) const
{
    ListRuleID result;
    // find first left rule
    int i = 0;
    for (;i<ids.size(); ++i) {
        if (!_ruleByID[ids.at(i)].rule._isRightRule)
            break;
    }
    if (i < ids.size()) {
        // add left rules (reverted)
        for (int j=ids.size() - 1;j >= i; --j)
            result.append(ids.at(j));
    }
    if (i > 0) {
        // add right rules
        for (int j=0; j < i; ++j)
            result.append(ids.at(j));
    }
    return result;
}


QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr)
{
    ds << gr._size;
    ds << gr._ruleByID;
    ds << gr._rulesByFeatureID;
    ds << gr._rulesByFeatureIDReverse;
    ds << gr._rootScore;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr)
{
    if (!gr._ruleByID.isEmpty()) {
        qWarning("Trying to deserialize CNFGrammar which is not empty (Tip: call clear() )");
        return ds;
    }
    ds >> gr._size;
    ds >> gr._ruleByID;
    ds >> gr._rulesByFeatureID;
    ds >> gr._rulesByFeatureIDReverse;
    ds >> gr._rootScore;

    gr.resizeMatrix(gr._size);
    for (int i=0; i < gr._ruleByID.size(); ++i)
        gr.fillCache(i);

    return ds;
}
