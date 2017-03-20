#include "cnfgrammar.h"

#include "rutextparser_extensions.h"

#include <QFile>
#include <QStringList>

#include <QDebug>


CNFGrammar::CNFGrammar()
{
    initDumpFilename();
}

CNFGrammar::CNFGrammar(const featureID &featureCount)
{
    initDumpFilename();

    // init vectors
    _rootScore.resize(featureCount);
    _rulesByFeatureID.resize(featureCount);
    // init matrix
    _rulesByRightIDsHash.resize(featureCount);
    for (int i=0; i < featureCount; ++i)
        _rulesByRightIDsHash[i].resize(featureCount);
}

CNFGrammar::~CNFGrammar()
{
    clear();
}

void CNFGrammar::append(const RuleRecord &rule)
{
    QList<ruleID> ruleIDs;
    foreach (const RuleDepend &depend, rule.depends()) {
        ruleIDs.append( insert(
                    ChomskyRuleRecord(depend.link, depend.terminal,
                                      rule.sourceRule(), depend.isRight)));
    }
    std::sort(ruleIDs.begin(), ruleIDs.end());

    insert(rule.sourceRule(), ruleIDs);
}

void CNFGrammar::addRoot(const featureID &fid)
{
    _rootScore[fid].increaseScore();
}



void CNFGrammar::clear()
{
    clearCache();

    _ruleByID.clear();
    _rulesByFeatureID.clear();
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

    _rulesByRightIDsHash[rule.leftID()][rule.rightID()].append(
                ScoredRuleID(newID));
}

ruleID CNFGrammar::insert(const ChomskyRuleRecord &rule)
{
    ruleID foundID = find(rule);
    if (foundID != -1) {
        _ruleByID[foundID].increaseScore();
        return foundID;
    }

    ruleID newID = _ruleByID.size();
    // insert new rule
    _ruleByID.append(ScoredChomskyRuleRecord(rule));
    // fill cache
    fillCache(newID);
    return newID;
}

ruleID CNFGrammar::find(const ChomskyRuleRecord &rule) const
{
    const ListScoredRuleID &rules = _rulesByRightIDsHash[rule.leftID()][rule.rightID()];

    foreach (const ScoredRuleID &id, rules) {
        if (_ruleByID[id.id].rule.sourceRule == rule.sourceRule)
            return id.id;
    }
    // not found
    return -1;
}

void CNFGrammar::insert(const featureID &srcRuleID, const ListRuleID &ids)
{
    int innerIndex = find(srcRuleID, ids);
    if (innerIndex != -1) {
        _rulesByFeatureID[srcRuleID][innerIndex].increaseScore();
    }
    else {
        // insert new rule
        ExtensionsQtContainers::insert_sorted(_rulesByFeatureID[srcRuleID], ScoredListRuleID(ids));
    }
}

int CNFGrammar::find(const featureID &srcRuleID, const ListRuleID &ids) const
{
    const ListScoredListRuleID &ruleList = _rulesByFeatureID[srcRuleID];
    for (int i=0; i < ruleList.size(); ++i) {
        if (ruleList[i].list == ids)
            return i;
    }
    // not found
    return -1;
}

QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr)
{
    ds << gr._dumpFilename;
    ds << gr._ruleByID;
    ds << gr._rulesByFeatureID;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr)
{
    if (!gr._ruleByID.isEmpty()) {
        qWarning("Trying to deserialize CNFGrammar which is not empty (Tip: call clear() )");
        return ds;
    }

    ds >> gr._dumpFilename;
    ds >> gr._ruleByID;
    ds >> gr._rulesByFeatureID;

    for (int i=0; i < gr._ruleByID.size(); ++i)
        gr.fillCache(i);

    return ds;
}
