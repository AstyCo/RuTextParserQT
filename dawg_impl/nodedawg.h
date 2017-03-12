#ifndef NODEDAWG_H
#define NODEDAWG_H

#include <QMultiMap>

<typename TValue>
struct NodeDAWG
{
    TValue value;
    int weight;

    QMultiMap<TValue, NodeDAWG*> childs;
    QMultiMap<TValue, NodeDAWG*> parents;

public:
    NodeDAWG(const TValue &val,NodeDAWG *par = NULL) : value(val), weight(1)
    {
        addParent(par);
    }
    ~NodeDAWG()
    {
        foreach (NodeDAWG *child, childs){
            if (!--child->_linkCnt)
                delete child;
        }
    }

    void addChild(NodeDAWG *child)
    {
        if (child) {
            childs.insert(child->value, child);
            child->addParent(this);
        }
    }
    void removeChild(NodeDAWG *child)
    {
        if (child) {
            childs.remove(child->value, child);
            child->removeParent(this);
        }
    }


    void addParent(NodeDAWG *parent)
    {
        if (parent) {
            ++_linkCnt;
            parents.insert(parent->value, parent);
        }
    }
    void removeParent(NodeDAWG *parent)
    {
        if (parent) {
            --_linkCnt;
            parents.remove(parent->value, parent);
        }
    }

private:
    int _linkCnt : 16;
};

#endif // NODEDAWG_H
