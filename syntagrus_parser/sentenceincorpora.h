#ifndef SENTENCEINCORPORA_H
#define SENTENCEINCORPORA_H

#include "recordincorpora.h"

#include <QList>

struct RecordNode
{
    RecordNode *_parent;
    QList<RecordNode *> _childNodes;

    RecordInCorpora _record;
public:
    RecordNode(const RecordInCorpora &initRecord = RecordInCorpora(),
               RecordNode *parentNode = NULL);

    RecordNode *parent() const;
    void setParent(RecordNode *parent);

    RecordInCorpora &record();
    QList<RecordNode *> &childNodes();
    void append(RecordNode *node);
};

class SentenceInCorpora
{
    QString _error;
    RecordNode *_root;
public:
    SentenceInCorpora(RecordNode *rootRecord = NULL);

    bool isValid() const;
    QString error() const;
    void setError(const QString &error);

    RecordNode *root() const;
    void setRoot(RecordNode * const root);

    RecordNode *nodeById(int id) const;

private:
    template <typename T>
    RecordNode *nodeH(const T &value, RecordNode *node,
                      bool (*compareFunction)(const RecordInCorpora &, const T &) ) const;
};

template<typename T>
RecordNode *SentenceInCorpora::nodeH(
        const T &value,
        RecordNode *node,
        bool (*compareFunction)(const RecordInCorpora &, const T &)
        ) const
{
    if (!node)
        return NULL;

    if (compareFunction(node->record(), value))
        return node;

    foreach (RecordNode *childNode, node->childNodes())
        if (RecordNode *nodeFoundInChild = nodeH(value, childNode, compareFunction)) {
            return nodeFoundInChild;
        }

    return NULL;
}



/// INLINE FUNCTIONS

inline bool SentenceInCorpora::isValid() const { return _error.isEmpty(); }

inline QString SentenceInCorpora::error() const{ return _error;}

inline void SentenceInCorpora::setError(const QString &error) { _error = error; }

#endif // SENTENCEINCORPORA_H
