#include "sentenceincorpora.h"

RecordNode::RecordNode(const RecordInCorpora &initRecord,
                       RecordNode *parentNode)
    : _parent(parentNode), _record(initRecord)
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

QList<RecordNode *> &RecordNode::childNodes()
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




RecordNode *SentenceInCorpora::root() const
{
    return _root;
}

void SentenceInCorpora::setRoot(RecordNode * const root)
{
    _root = root;
}

bool compareById(const RecordInCorpora &record, const int &id)
{
    return record._id == id;
}



RecordNode *SentenceInCorpora::nodeById(int id) const
{
    return nodeH(id, _root, compareById);
}

//RecordNode *SentenceInCorpora::nodeH(int id, RecordNode *node) const
//{
//    if (!node)
//        return NULL;

//    if (node->record()._id == id)
//        return node;

//    foreach (RecordNode *childNode, node->childNodes())
//        if (RecordNode *nodeFoundInChild = nodeH(id, childNode)) {
//            return nodeFoundInChild;
//        }

//    return NULL;
//}

SentenceInCorpora::SentenceInCorpora(RecordNode *rootRecord)
    : _root(rootRecord)
{

}
