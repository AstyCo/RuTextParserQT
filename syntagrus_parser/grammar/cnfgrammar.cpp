#include "cnfgrammar.h"

#include "rutextparser_extensions.h"

#include <QFile>
#include <QStringList>

#include <QDebug>

QDebug operator<< (QDebug d, const RuleCNFGrammar &r) {
    if (r._type == RuleCNFGrammar::TerminalRule) {
        d << r._leftNonterminal
          << "->"
          << r._terminal;
    }
    else {
        d << r._leftNonterminal
          << "->"
          << r._firstNonterminal
          << ' '
          << r._secondNonterminal;
    }

    return d;
}


CNFGrammar::CNFGrammar(const featureID &featureCount)
{
    static QString dumpFilename("dumps/cnfgrammar.dump");

#ifdef RUTEXTPARSERQT_DIR
    _dumpFilename = RUTEXTPARSERQT_DIR + dumpFilename;
#else
    qWarning("RUTEXTPARSERQT_DIR not defined");

    QString manualPath = "../../RuTextParserQT/";
    _dumpFilename = manualPath + dumpFilename;
#endif

    // init vector
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

    insert(rule.sourceRule(), ruleIDs);
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

NonterminalList CNFGrammar::getNonterminals(const Terminal &terminal) const
{
    NonterminalList nonterminals;
    QMultiHash<Terminal, const RuleCNFGrammar *>::const_iterator
            i(_mappingTerminalToRules.constFind(terminal));

//    qDebug() << QString("looking for rule[X -> %1]").arg(terminal);
    while (i != _mappingTerminalToRules.constEnd() && i.key() == terminal) {
//        qDebug() << "terminal key" << i.key();
//        qDebug() << "terminal value" << *i.value();
        nonterminals.append(i.value()->_leftNonterminal);
        ++i;
    }

    return nonterminals;
}

NonterminalList CNFGrammar::getNonterminals(const Nonterminal &first, const Nonterminal &second) const
{
//    qDebug() << QString("looking for rule[X -> %1 %2]")
//                .arg(QString::number(first))
//                .arg(QString::number(second));

    NonterminalList nonterminals;
    NonterminalPair key = concatTerminals(first, second);
    QMultiHash<NonterminalPair, const RuleCNFGrammar *>::const_iterator i(_mappingRightToRules.find(key));

    while (i != _mappingRightToRules.constEnd() && i.key() == key) {
        nonterminals.append(i.value()->_leftNonterminal);
        ++i;
    }

    return nonterminals;
}

NonterminalList CNFGrammar::getNonterminals(const NonterminalList &firsts, const NonterminalList &seconds) const
{
    NonterminalList res;
    qDebug() << QString("getNonterminals fsz[%1] ssz[%2]")
                .arg(QString::number(firsts.size()))
                .arg(QString::number(seconds.size()));
//    qDebug() << "firsts" << firsts;
//    qDebug() << "seconds" << seconds;


    foreach (const Nonterminal &f, firsts)
        foreach (const Nonterminal &s, seconds) {
            res.append(getNonterminals(f, s));
        }

//    qDebug() << "res sz" << res.size();
    return res;
}

QStringList CNFGrammar::toSentence(const QVector<NonterminalList> &row) const
{
    QStringList res;
    foreach (const NonterminalList &nlist, row ){
//        qDebug() << "tosent";

        QMultiHash<Nonterminal, const RuleCNFGrammar *>::const_iterator i = _mappingLeftToRules.constFind(nlist.first());
        Q_ASSERT(i != _mappingLeftToRules.constEnd() && i.key() == nlist.first());
        QString feat = i.value()->_terminal;

        Q_ASSERT(!feat.isEmpty());
        res.append(feat);
    }
    return res;
}

NonterminalPair CNFGrammar::concatTerminals(const Nonterminal &first, const Nonterminal &second)
{
    return qMakePair(first, second);
}

void CNFGrammar::clearCache()
{
    _rulesByRightIDsHash.clear();
}

void CNFGrammar::fillCache(RuleCNFGrammar *p_rule)
{

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
    _rulesByRightIDsHash[rule.leftID()][rule.rightID()].append(ScoredRuleID(newID));
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
        int newIndex = _rulesByFeatureID[srcRuleID].size();
        _rulesByFeatureID[srcRuleID].append(ScoredListRuleID(ids));
    }
}

int CNFGrammar::find(const featureID &srcRuleID, const ListRuleID &ids) const
{
    const ListScoredListRuleID &ruleList = _rulesByFeatureID[srcRuleID];
    for (int i=0; i < ruleList.size(); ++i) {
        if (ruleList[i] == ids)
            return i;
    }
    // not found
    return -1;
}

QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr)
{
    ds << gr._ruleByID.size();
    for (int i = 0; i < gr._ruleByID.size(); ++i)
        ds << *gr._ruleByID[i];

    return ds;
}

QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr)
{
    if (!gr._ruleByID.isEmpty()) {
        qWarning("Trying to deserialize CNFGrammar which is not empty (Tip: call clear() )");
        return ds;
    }

    int rulesSize;
    ds >> rulesSize;
    Q_ASSERT(rulesSize >= 0);

    gr._ruleByID.resize(rulesSize);
    for (int i = 0; i < rulesSize; ++i){
        gr._ruleByID[i] = new RuleCNFGrammar();
        ds >> *gr._ruleByID[i];
    }
    for (int i = 0; i < gr._ruleByID.size(); ++i) {
        gr.fillCache(gr._ruleByID[i]); // fills mapping
    }

    return ds;
}
