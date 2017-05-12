#include "rulenode.h"
#include "grammar/cnfgrammar.h"
#include "syntagrus_parser/grammar/simpleruletree.h"

#include <QtMath>


RuleLink::RuleLink(ruleID rid, QSharedPointer<RuleNode> ruleNode)
    : id(rid), node(ruleNode)
{

}

bool RuleLink::operator==(const RuleLink &other) const
{
    return id == other.id && *node == *other.node;
}

//const RuleLink &RuleLink::operator=(const RuleLink &other)
//{
//    id = other.id;
//    node = QSharedPointer<RuleNode>(new RuleNode(*other.node));
//}

//long RuleNode::callgrind_debug = 0;

QString RuleNode::toString(const QVector<ScoredChomskyRuleRecord> &ruleByID, const FeatureMapper &fmapper, const LinkMapper &lmapper, int tab) const
{
    QString result;
    QString tabstring;
    for (int i=0; i<tab; ++i)
        tabstring += " ";

//    const ChomskyRuleRecord &ruleGrammar = ruleByID.at(ruleNode->rules().first().id).rule;
//    QString srcFeat = fmapper.value(ruleGrammar._sourceFID);

//    QList<HelperCompare> temp;
//    for (int i=0; i < ruleNode->rules().size(); ++i) {
//        const RuleLink &childLink = ruleNode->rules().at(i);
//        HelperCompare helper;
//        const ChomskyRuleRecord &rule = ruleByID.at(childLink.id).rule;
//        helper.isRight = rule._isRightRule;
//        helper.link = lmapper.value(rule._linkID);
//        helper.feat = fmapper.value(rule._dependFID);
//        helper.index = i;

//        temp.append(helper);
//    }
    if (!tab)
        result += tabstring + "RULE{\n";
    for (int i=0; i<rules().size(); ++i) {
        const RuleLink &childLink = rules().at(i);
        result += tabstring + ruleByID.at(childLink.id).rule.toString(fmapper, lmapper) + "\n";
        result += childLink.node->toString(ruleByID, fmapper, lmapper, tab+4);
    }
    if (!tab)
        result += tabstring + "}\n";
    return result;
//    std::sort(temp.begin(), temp.end(), lessThan2);
}

const RuleNode &RuleNode::operator=(const RuleNode &other)
{
    _delta = other._delta;
    _rules = other._rules;

    return *this;
}

RuleNode::RuleNode(const RuleNode &other)
    : Scored(other.score), _delta(other._delta), _rules(other._rules)
{
//    callgrind_debug++;
}

RuleNode::RuleNode(qreal scoreValue, quint8 delta)
    : Scored(scoreValue), _delta(delta)
{
//    callgrind_debug++;
}

RuleNode::RuleNode(ruleID rid, QSharedPointer<RuleNode> child, qreal scoreValue, quint8 delta)
    : Scored(scoreValue), _delta(child->_delta + delta)
{
//    callgrind_debug++;

    _rules.append(RuleLink(rid, child));
}

RuleNode::~RuleNode()
{
//    callgrind_debug--;
    //    qDebug() << "callgrind_debug" << callgrind_debug;
}

const SimpleRuleNode *RuleNode::grammarGetNodeWidth(const featureID &fid, const CNFGrammar &grammar) const
{
    ListRuleID ordered;
    for (int i=0; i<_rules.size(); ++i)
        ordered.append(_rules[i].id);

    return grammar.rulesByFeatureID()[fid].node(ordered);
}

qreal RuleNode::calcProb(ruleID prid, const CNFGrammar &grammar,
                         const FeatureMapper &fmapper, const LinkMapper &lmapper) const
{
    qreal result = 0;
    const QVector<ScoredChomskyRuleRecord> &ruleByID = grammar.rulesByID();
    const SimpleRuleTree &seqTree = grammar.conseqRules();
    foreach (const RuleLink &link , _rules) {
        qreal ruleScore = ruleByID[link.id].score;

        featureID fid_src = ruleByID[link.id].rule._sourceFID;
        const SimpleRuleTree &tree = grammar.rulesByFeatureID()[fid_src];
        result += /*qLn(*/ruleScore /** tree.totalCount() *1. / tree.totalScore()*//*)*/;
        result += link.node->calcProb(link.id, grammar, fmapper, lmapper);
    }

    return result;
}

qreal RuleNode::calcProb2(ruleID last, const CNFGrammar &grammar, const FeatureMapper &fmapper, const LinkMapper &lmapper) const
{
    qreal result = 0;
    if (_rules.isEmpty())
        return result;
    const QVector<ScoredChomskyRuleRecord> &rulesByID = grammar.rulesByID();
    const ChomskyRuleRecord &srcRule = rulesByID[_rules.first().id].rule;

    if (last == -1) {
        // root
        featureID srcFid = srcRule._sourceFID;
        const QMap<QPair<featureID,ruleID>, int> &conseqRootRules = grammar.conseqRootRules();
//        result += qLn(grammar.rootScore()[srcFid].score / grammar.rootCount());

        foreach (const RuleLink &link , _rules) {
            result += qLn(10 + conseqRootRules[qMakePair(srcFid, link.id)] *1. / grammar.rootRulesCount());
            result += link.node->calcProb2(link.id, grammar, fmapper, lmapper);
        }
    }
    else if (last!=-1) {
        const ChomskyRuleRecord &rule = rulesByID[last].rule;
        const QMap<QPair<QPair<linkID, featureID>,ruleID>, int> &conseqRules = grammar.conseqRulesV2();

        foreach (const RuleLink &link , _rules) {
            qreal score;
            QList<ruleID> ids;

            if (!conseqRules.contains(qMakePair(qMakePair(rule._linkID, rule._dependFID),link.id))) {
                Q_ASSERT(false);
            }

            score = 10 + conseqRules[qMakePair(qMakePair(rule._linkID, rule._dependFID),link.id)]
                    *1. / grammar.conseqRulesCount();
            result += qLn(score);
            result += link.node->calcProb2(link.id, grammar, fmapper, lmapper);
        }
//    }
    }
    return result;
}

void RuleNode::append(const RuleLink &link)
{
    _delta += link.node->_delta;
//    ExtensionsQtContainers::insert_sorted(_rules, link);
    _rules.append(link);
}

void RuleNode::append(const QList<RuleLink> &links)
{
    foreach (const RuleLink &link, links)
        append(link);
}

const QList<QPair<featureID, linkID> > RuleNode::toLAS(const CNFGrammar &grammar) const
{
    QPair<QList<QPair<featureID, linkID> >,QList<QPair<featureID, linkID> > > tmp = toLASH(grammar);
    tmp.first.append(tmp.second);
    return tmp.first;
}

const QList<featureID> RuleNode::toUAS(const CNFGrammar &grammar) const
{
    QList<QPair<featureID, linkID> > las = toLAS(grammar);
    QList<featureID> result;

    for (int i=0; i < las.size(); ++i)
        result.append(las[i].first);

    return result;
}

const QPair<QList<QPair<featureID, linkID> >,QList<QPair<featureID, linkID> > >
    RuleNode::toLASH(const CNFGrammar &grammar) const
{
    QPair<QList<QPair<featureID, linkID> >,QList<QPair<featureID, linkID> > > result;

    for (int i=0; i < _rules.size(); ++i) {
        const RuleLink &link = _rules[i];
        const ChomskyRuleRecord &rule = grammar.rulesByID()[link.id].rule;
        QPair<QList<QPair<featureID, linkID> >,QList<QPair<featureID, linkID> > > tmp
                = link.node->toLASH(grammar);

        if (rule._isRightRule){
            result.second.append(tmp.first);
            result.second.append(qMakePair(rule._sourceFID, rule._linkID));
            result.second.append(tmp.second);
        }
        else {
            result.first.append(tmp.first);
            result.first.append(qMakePair(rule._sourceFID, rule._linkID));
            result.first.append(tmp.second);
        }
    }
    return result;
}



