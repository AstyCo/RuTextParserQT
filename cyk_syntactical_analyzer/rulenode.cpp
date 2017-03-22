#include "rulenode.h"


RuleLink::RuleLink(ruleID rid, QSharedPointer<RuleNode> ruleNode)
    : id(rid), node(ruleNode)
{

}

bool RuleLink::operator==(const RuleLink &other) const
{
    return id == other.id && *node == *other.node;
}



RuleNode::RuleNode(qreal scoreValue)
    : Scored(scoreValue)
{

}

RuleNode::RuleNode(ruleID rid, QSharedPointer<RuleNode> child, qreal scoreValue)
    : Scored(scoreValue)
{
    rules.append(RuleLink(rid, child));
}

