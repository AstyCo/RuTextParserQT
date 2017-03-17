#ifndef DAWGIMPL_H
#define DAWGIMPL_H

#include "nodedawg.h"

<typename TValue>
class AstyDAWG
{
    typedef NodeDAWG<TValue> Node;

    Node _root;
    QMap<TValue, Node*> _lasts;
public:
    AstyDAWG();

//    void append(const QList<TValue> &record)
//    {
//        Node *p_node = &_root;

//        for (int i=0; i < arraySize; ++i) {

//            if (!p_node->childs.contains(array[i])) {
//                p_node->insert(array + i, arraySize - i);
////                p_node->childs.insert(array[i], makeNodes(array, arraySize, i, p_node));
//            }

//        }
//    }

    QList<Node*> commonPrefix(const QList<TValue> &word)
    {
        QList<Node*> result;
        result.append(&_root);

        QList<TValue>::const_iterator i(word.constBegin());

        while(i != word.constEnd()) {
            if ((*i).childs.contains())

            ++i;
        }
    }

};



#endif // DAWGIMPL_H
