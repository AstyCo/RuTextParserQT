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

bool compareByUniqueRootScore(const QPair<featureID, SimpleRuleTree> &lhs, const QPair<featureID, SimpleRuleTree> &rhs)
{
    return lhs.second.totalRootCount() > rhs.second.totalRootCount();
}

bool compareByRootScore(const QPair<featureID, SimpleRuleTree> &lhs, const QPair<featureID, SimpleRuleTree> &rhs)
{
    return lhs.second.totalRootScore() > rhs.second.totalRootScore();
}

bool compareByUniqueScore(const QPair<featureID, SimpleRuleTree> &lhs, const QPair<featureID, SimpleRuleTree> &rhs)
{
    return lhs.second.totalCount() > rhs.second.totalCount();
}
bool compareByScore(const QPair<featureID, SimpleRuleTree> &lhs, const QPair<featureID, SimpleRuleTree> &rhs)
{
    return lhs.second.totalScore() > rhs.second.totalScore();
}

QString CNFGrammar::toReport(const FeatureMapper &fmapper) const {
    QList<QPair<featureID, SimpleRuleTree> > sortedRulesByFeatureID;
    for (featureID i=0; i < _size; ++i)
        sortedRulesByFeatureID.append(qMakePair(i, _rulesByFeatureID[i]));

    std::sort(sortedRulesByFeatureID.begin(), sortedRulesByFeatureID.end(), compareByScore);

    qreal totalTopRules = 0;
    foreach (const SimpleRuleTree &tree, _rulesByFeatureID)
        totalTopRules += tree.totalScore();

    QString result(QString("TOTAL RULES:%1\n").arg(totalTopRules));

    for (int i=0; i < _size; ++i) {
        result += QString("\t%1: %2\n")
                .arg(fmapper.value(sortedRulesByFeatureID[i].first))
                .arg(sortedRulesByFeatureID[i].second.totalScore());
    }
    qreal totalTopRulesCount = 0;
    foreach (const SimpleRuleTree &tree, _rulesByFeatureID)
        totalTopRulesCount += tree.totalCount();

    std::sort(sortedRulesByFeatureID.begin(), sortedRulesByFeatureID.end(), compareByUniqueScore);
    result += QString("\nTOTAL UNIQUE RULES:%1\n").arg(totalTopRulesCount);
    for (int i=0; i < _size; ++i) {
        result += QString("\t%1: %2\n")
                .arg(fmapper.value(sortedRulesByFeatureID[i].first))
                .arg(sortedRulesByFeatureID[i].second.totalCount());
    }


    QList<QPair<featureID, SimpleRuleTree> > sortedRoots;
    for (featureID i=0; i < _size; ++i)
        sortedRoots.append(qMakePair(i, _rulesByFeatureID[i]));

    std::sort(sortedRoots.begin(), sortedRoots.end(), compareByRootScore);

    qreal totalRootRules = 0;
    foreach (const SimpleRuleTree &tree, _rulesByFeatureID)
        totalRootRules += tree.totalRootScore();

    result += QString("\nTOTAL ROOT RULES:%1\n").arg(totalRootRules);
    for (int i=0; i < _size; ++i) {
        result += QString("\t%1: %2\n")
                .arg(fmapper.value(sortedRoots[i].first))
                .arg(sortedRoots[i].second.totalRootScore());
    }
    qreal totalRootCount = 0;
    foreach (const SimpleRuleTree &tree, _rulesByFeatureID)
        totalRootCount += tree.totalRootCount();

    std::sort(sortedRoots.begin(), sortedRoots.end(), compareByUniqueRootScore);
    result += QString("\nTOTAL UNIQUE ROOT RULES:%1\n").arg(totalRootCount);
    for (int i=0; i < _size; ++i) {
        result += QString("\t%1: %2\n")
                .arg(fmapper.value(sortedRoots[i].first))
                .arg(sortedRoots[i].second.totalRootCount());
    }


    return result;
}

void CNFGrammar::append(const UniqueVector<featureID, QString> &fmapper,
                        const UniqueVector<linkID, QString> &lmapper,
                        const RecordNode *node,
                        const ruleID &parentID)
{
    if (!node) {
        printWarning("TreeParser::parseNode: null node");
        return;
    }

    featureID fid = fmapper.index(node->record()._feat);
    bool root = false;
    if (node->record()._dom == -1) {
//        Q_ASSERT(parentID<0);
        // root rule
//        addRoot(fid);
        root = true;
    }

    if (!node->childNodes().isEmpty()) {

        RuleRecord rule(fid);

        ListRuleID ruleIDs;
        for (int i=0; i < node->childNodes().size(); ++i) {
            linkID lid = lmapper.index(node->childNodes().at(i)->record()._link);
            featureID childFid = fmapper.index(node->childNodes().at(i)->record()._feat);

            ruleID rid = insert(ChomskyRuleRecord(lid, childFid, fid,
                                                  node->record().before(node->childNodes().at(i)->record())));

            ruleIDs.append(rid);
            // add in depth rule
//            if (parentID >= 0)
            insertInDepthRule(rid, parentID);

            // append recursively
            append(fmapper, lmapper, node->childNodes().at(i), rid);
        }
        ListRuleID cykFormed = produceSequenceForCYK(ruleIDs);
        insertWidthRule(fid, cykFormed, root);
    }
}

//void CNFGrammar::finishBuilding()
//{
//    Q_ASSERT(_plistRulesByFID != NULL);

//    for (int i=0; i<_size; ++i)
//        _rulesByFeatureID[i] = listScoredToSetScored(_plistRulesByFID->at(i));

//    delete [] _plistRulesByFID;
//    _plistRulesByFID = NULL;
//}


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
    _conseqRules.clear();
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

void CNFGrammar::insertWidthRule(const featureID &srcRuleID, const ListRuleID &ids, bool rootRule)
{
    if (ids.isEmpty())
        Q_ASSERT(false);
    // Simple insert
    SimpleRuleNode *node = _rulesByFeatureID[srcRuleID].node(ids);
    if (node) {
        // increase score of existing rule
        node->increaseScore();
        if (rootRule)
            node->rootScore.increaseScore();
    }
    else {
        // insert new rule
        _rulesByFeatureID[srcRuleID].insert(ids, rootRule);
    }
}

void CNFGrammar::insertInDepthRule(const ruleID &rid, const ruleID &prid)
{
    QList<ruleID> list;
    list.append(rid);
    list.append(prid);

    _conseqRules.insert(list, false);
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
    ds << gr._conseqRules;
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
    ds >> gr._conseqRules;
    ds >> gr._rootScore;

    gr.resizeMatrix(gr._size);
    for (int i=0; i < gr._ruleByID.size(); ++i)
        gr.fillCache(i);

    return ds;
}
