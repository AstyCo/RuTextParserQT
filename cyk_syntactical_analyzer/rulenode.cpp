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

qreal RuleNode::calcProb(const CNFGrammar &grammar,
                         const FeatureMapper &fmapper, const LinkMapper &lmapper) const
{
    qreal result = 0;
    const QVector<ScoredChomskyRuleRecord> &ruleByID = grammar.rulesByID();
    foreach (const RuleLink &link , _rules) {
        result += qLn(ruleByID[link.id].score);
        result += link.node->calcProb(grammar, fmapper, lmapper);
    }
//    if (_rules.isEmpty())
//        return result;
//    QList<ruleID> ids;
//    for (int i=0; i < _rules.size(); ++i)
//        ids << _rules.at(i).id;
//    featureID fid = grammar.rulesByID()[_rules.first().id].rule._sourceFID;
//    const SimpleRuleNode *node = grammar.rulesByFeatureID()[fid].node(ids);
//    if (node && node->score > 0.5) {
//        result += qLn(node->score);
//    }
//    else {
//        qDebug().noquote() << toString(grammar.rulesByID(), fmapper, lmapper);
//        Q_ASSERT(node);
//        Q_ASSERT(node->score > 0.5);
//    }
//    foreach (const RuleLink &link, _rules) {
//        result += link.node->calcProb(grammar, fmapper, lmapper);
//    }

    return result;
}

qreal RuleNode::calcProb2(ruleID last, const CNFGrammar &grammar, const FeatureMapper &fmapper, const LinkMapper &lmapper, int var) const
{
    qreal result = 0;
    if (_rules.isEmpty())
        return result;
    if (last == -1) {
        // calculating root
        switch (var) {
        case 1:
        {
            const SimpleRuleNode *node = grammarGetNodeWidth(grammar.rulesByID()[_rules.first().id].rule._sourceFID, grammar);
            Q_ASSERT(node!=NULL);

            result += /*qLn(*/node->rootScore.score/*)*/;
            foreach (const RuleLink &link, _rules)
                result += link.node->calcProb2(link.id, grammar, fmapper, lmapper, var);
            break;
        }
        case 2:
        {
            const QVector<ScoredChomskyRuleRecord> &ruleByID = grammar.rulesByID();
            foreach (const RuleLink &link , _rules) {
                result += qLn(ruleByID[link.id].score);
                result += link.node->calcProb2(link.id, grammar, fmapper, lmapper, var);
            }
            break;
        }
        }
    }
    else {
        const SimpleRuleTree &conseqRules = grammar.conseqRules();

        foreach (const RuleLink &link , _rules) {
            qreal score;
            QList<ruleID> ids;
            ids << link.id << last;
            if (!conseqRules.contains(ids))
                score = 1;
            else
                score = 1 + conseqRules.node(ids)->score;
            result += /*qLn(*/score/*)*/;
            result += link.node->calcProb2(link.id, grammar, fmapper, lmapper, var);
        }
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


