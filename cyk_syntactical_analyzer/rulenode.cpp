#include "rulenode.h"


RuleLink::RuleLink(ruleID rid, QSharedPointer<RuleNode> ruleNode)
    : id(rid), node(ruleNode)
{

}

bool RuleLink::operator==(const RuleLink &other) const
{
    return id == other.id && *node == *other.node;
}

long RuleNode::callgrind_debug = 0;

RuleNode::RuleNode(qreal scoreValue, quint8 delta)
    : Scored(scoreValue), _delta(delta)
{
    callgrind_debug++;
}

RuleNode::RuleNode(ruleID rid, QSharedPointer<RuleNode> child, qreal scoreValue, quint8 delta)
    : Scored(scoreValue), _delta(child->_delta + delta)
{
    callgrind_debug++;

    _rules.append(RuleLink(rid, child));
}

RuleNode::~RuleNode()
{
    callgrind_debug--;
    qDebug() << "callgrind_debug" << callgrind_debug;
}

bool RuleNode::insert(const RuleLink &link)
{
    _delta += link.node->_delta;
    ExtensionsQtContainers::insert_sorted(_rules, link);
}

