#ifndef SENTENCEINCORPORA_H
#define SENTENCEINCORPORA_H

#include "syntagrus_parser_global.h"

#include "recordincorpora.h"
#include "recordnode.h"
#include "wordincorpora.h"
#include "sentenceinfo.h"

#include <QList>
#include <QVector>
#include <QStringList>
#include <QDataStream>




class SYNTAGRUS_PARSERSHARED_EXPORT SentenceInCorpora
{
    QString _error;
    SentenceInfo _qDebugSentence;
    bool _skip;
    RecordNode *_root;
public:
    SentenceInCorpora(RecordNode *rootRecord = NULL);

    inline bool isValid() const;
    inline QString error() const;
    inline void setError(const QString &error);

    RecordNode *root() const;
    void setRoot(RecordNode * const root);
    void updateSentence();

    RecordNode *nodeById(int id) const;

    const SentenceInfo &qDebugSentence() const;
    int size() const;

    void setSkip();
    bool skip() const;

    friend QDataStream &operator<<(QDataStream &ds, const SentenceInCorpora &s);
    friend QDataStream &operator>>(QDataStream &ds, SentenceInCorpora &s);

private:
    void fillSentenceInfo(RecordNode * node = NULL);
    template <typename T>
    RecordNode *nodeH(const T &value, RecordNode *node,
                      bool (*compareFunction)(const RecordInCorpora &, const T &) ) const;
};

QDataStream &operator<<(QDataStream &ds, const SentenceInCorpora &s);
QDataStream &operator>>(QDataStream &ds, SentenceInCorpora &s);

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
