#include "recordnode.h"
#include "rutextparser_extensions/rutextparser_extensions.h"

#include <QDebug>

RecordNode::RecordNode(const RecordInCorpora &initRecord,
                       RecordNode *parentNode)
    : _record(initRecord), _parent(parentNode)
{
}
RecordNode *RecordNode::parent() const
{
    return _parent;
}

void RecordNode::setParent(RecordNode *parent)
{
    _parent = parent;
}

RecordInCorpora &RecordNode::record()
{
    return _record;
}

const RecordInCorpora &RecordNode::record() const
{
    return _record;
}

QList<RecordNode *> &RecordNode::childNodes()
{
    return _childNodes;
}

const QList<RecordNode *> &RecordNode::childNodes() const
{
    return _childNodes;
}

void RecordNode::append(RecordNode *node)
{
    if (!node)
        return;

    node->setParent(this);

    _childNodes.append(node);
}

bool RecordNode::isProjective() const
{
    if (_childNodes.isEmpty())
        return true;

    foreach (const RecordNode *child, _childNodes)
        if (!child->isProjective())
            return false;

    QList<int> sortedIndexes = indexes();
    std::sort(sortedIndexes.begin(), sortedIndexes.end());

    int lastIndex = sortedIndexes.first();
    for (int i=1; i < sortedIndexes.size(); ++i) {
        if (sortedIndexes[i] - lastIndex != 1)
            return false;
        lastIndex = sortedIndexes[i];
    }

    return true;
}

// not sorted
QList<int> RecordNode::indexes() const
{
    QList<int> result;
    result << _record._id;

    if (_childNodes.isEmpty())
        return result;

    foreach (const RecordNode *child, _childNodes)
        result << child->indexes();

    return result;
}

int RecordNode::size() const
{
    int result = 1;
    foreach(const RecordNode *childNode, _childNodes)
        result += childNode->size();

    return result;
}




QDataStream &operator<<(QDataStream &ds, const RecordNode &r)
{
    ds << r._record;

    for (int i = 0; i < r._childNodes.size(); ++i) {
        ds << true;
        ds << *r._childNodes.at(i);
    }

    ds << false; // no more childs
    return ds;
}

QDataStream &operator>>(QDataStream &ds, RecordNode &r)
{
    ds >> r._record;

    bool childExists;
    while (ds >> childExists, childExists) {
        RecordNode *rnode = new RecordNode();

        ds >> *rnode;

        r.append(rnode);
    }
    return ds;
}
