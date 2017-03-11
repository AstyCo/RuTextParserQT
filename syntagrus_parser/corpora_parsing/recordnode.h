#ifndef RECORDNODE_H
#define RECORDNODE_H

#include "recordincorpora.h"

#include <QList>
#include <QDataStream>

struct RecordNode
{
    RecordInCorpora _record;
    RecordNode *_parent;
    QList<RecordNode *> _childNodes;

public:
    RecordNode(const RecordInCorpora &initRecord = RecordInCorpora(),
               RecordNode *parentNode = NULL);

    RecordNode *parent() const;
    void setParent(RecordNode *parent);

    RecordInCorpora &record();
    const RecordInCorpora &record() const;
    QList<RecordNode *> &childNodes();
    const QList<RecordNode *> &childNodes() const;

    void append(RecordNode *node);

    int size() const;
    friend QDataStream &operator<<(QDataStream &ds, const RecordNode &r);
    friend QDataStream &operator>>(QDataStream &ds, RecordNode &r);
};

QDataStream &operator<<(QDataStream &ds, const RecordNode &r);
QDataStream &operator>>(QDataStream &ds, RecordNode &r);

#endif // RECORDNODE_H
