#include "simpleruletree.h"



SimpleRuleTree::SimpleRuleTree()
{
    _hashTotalCount = _hashTotalRootCount = _hashTotalRootScore = _hashTotalScore = -1;
}

void SimpleRuleTree::insert(const QList<ruleID> &list, bool rootRule)
{
    SimpleRuleNode *node = &_root;
    // all but last
    for (int i=0; i < list.size() - 1; ++i) {
        if (!node->node.contains(list.at(i)))
            node->node.insert(list.at(i), SimpleRuleNode(0));
        node = &node->node[list.at(i)];
    }
    if (!node)
        Q_ASSERT(false);
    // last
    {
        if (!node->node.contains(list.last()))
            node->node.insert(list.last(), SimpleRuleNode(1, (rootRule ? 1 : 0)));
        else {
            node->node[list.last()].increaseScore();
            if (rootRule)
                node->node[list.last()].rootScore.increaseScore();
        }
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







int SimpleRuleNode::totalCount() const
{
    int result = (score > 0.5 ? 1 : 0);
    foreach (const SimpleRuleNode &childNode, node)
        result += childNode.totalCount();
    return result;
}

int SimpleRuleNode::totalScore() const
{
    int result = score;
    foreach (const SimpleRuleNode &childNode, node)
        result += childNode.totalScore();
    return result;
}

int SimpleRuleNode::totalRootScore() const
{
    int result = rootScore.score;
    foreach (const SimpleRuleNode &childNode, node)
        result += childNode.totalRootScore();
    return result;
}

int SimpleRuleNode::totalRootCount() const
{
    int result = (rootScore.score>0.5 ? 1 : 0);
    foreach (const SimpleRuleNode &childNode, node)
        result += childNode.totalRootCount();
    return result;
}

void SimpleRuleNode::clear()
{
    score = 0;
    rootScore.score = 0;
    node.clear();
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
    ds << n.rootScore;
    ds << n.node;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, SimpleRuleNode &n)
{
    ds >> n.score;
    ds >> n.rootScore;
    ds >> n.node;

    return ds;
}
