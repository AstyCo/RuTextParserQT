#include "recordnode.h"

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
