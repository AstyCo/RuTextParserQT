#include "recordnode.h"

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
    if (childNodes().isEmpty())
        return true;

    // init ids
    QList<int> ids;
    bool rootIns = false;
    for (int i=0; i < _childNodes.size(); ++i) {
        const int &childId = _childNodes.at(i)->record()._id;
        if (!rootIns && childId > _record._id) {
            ids.append(_record._id);
            rootIns = true;
        }
        ids.append(childId);
    }
    if (!rootIns)
        ids.append(_record._id);
    qDebug() << "ids" << ids;

    // check for projective
    int last = ids.first();
    for (int i=1; i<ids.size(); ++i) {
        if (ids.at(i) - last != 1)
            return false;
        last = ids.at(i);
    }
    // recursive check too
    foreach (const RecordNode *child, _childNodes)
        if (!child->isProjective())
            return false;
    return true;
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
