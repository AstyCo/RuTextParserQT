#include "simpleruletree.h"



SimpleRuleTree::SimpleRuleTree()
{
}

void SimpleRuleTree::insert(const QList<ruleID> &list)
{
    SimpleRuleNode *node = &_root;
    for (int i=0; i < list.size(); ++i) {
        if (!node->node.contains(list.at(i)))
            node->node.insert(list.at(i), SimpleRuleNode((i==list.size()-1 ? 1 : 0)));
        node = &node->node[list.at(i)];
    }
}

bool SimpleRuleTree::contains(const QList<ruleID> &list) const
{
    return node(list) != NULL;
}

const SimpleRuleNode *SimpleRuleTree::node(const QList<ruleID> &list) const
{
    const SimpleRuleNode *node = &_root;
    QMap<ruleID, SimpleRuleNode>::const_iterator it;
    for (int i=0; i < list.size(); ++i) {
        it = node->node.find(list.at(i));
        if (it == node->node.constEnd())
            return NULL;
        node = &*it;
    }
    return &*it;
}

SimpleRuleNode *SimpleRuleTree::node(const QList<ruleID> &list)
{
    SimpleRuleNode *node = &_root;
    for (int i=0; i < list.size(); ++i) {
        if (!node->node.contains(list.at(i)))
            return NULL;
        node = &node->node[list.at(i)];
    }
    return node;
}

int SimpleRuleTree::size() const
{
    return _root.size();
}

int SimpleRuleNode::size() const
{
    int result = (score>0 ? 1 : 0);
    foreach (const SimpleRuleNode &childNode, node)
        result += childNode.size();
    return result;
}

QDataStream &operator<<(QDataStream &ds, const SimpleRuleTree &tree)
{
    ds << tree._root;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, SimpleRuleTree &tree)
{
    ds >> tree._root;

    return ds;
}

QDataStream &operator<<(QDataStream &ds, const SimpleRuleNode &n)
{
    ds << n.score;
    ds << n.node;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, SimpleRuleNode &n)
{
    ds >> n.score;
    ds >> n.node;

    return ds;
}
