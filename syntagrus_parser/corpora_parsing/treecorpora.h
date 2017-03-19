#ifndef TREECORPORA_H
#define TREECORPORA_H

#include "syntagrus_parser_global.h"

#include "recordincorpora.h"
#include "sentenceincorpora.h"
#include "uniquevector.h"
#include "internal/base-types.h"

#include <QVector>

typedef QVector<SentenceInCorpora> TextInCorpora;

class SYNTAGRUS_PARSERSHARED_EXPORT TreeCorpora
{
    QMultiHash<int, SentenceInCorpora> _sentencesBySize;
    UniqueVector<featureID, QString>  _featureMapper;
    UniqueVector<linkID, QString>  _linkMapper;

public:
    inline const QMultiHash<int, SentenceInCorpora> &sentencesBySize() const { return _sentencesBySize;}
    inline QMultiHash<int, SentenceInCorpora> &sentencesBySize() { return _sentencesBySize;}
    inline const UniqueVector<featureID, QString> &featureMapper() const { return _featureMapper;}
    inline const UniqueVector<linkID, QString> &linkMapper() const { return _linkMapper;}


    void append(const SentenceInCorpora &text);
    void append(const TextInCorpora &text);

    inline void clear() { _sentencesBySize.clear();}
    inline int size() const { return _sentencesBySize.size();}
    inline bool isEmpty() const { return _sentencesBySize.isEmpty();}


    friend QDataStream &operator<<(QDataStream &ds, const TreeCorpora &t);
    friend QDataStream &operator>>(QDataStream &ds, TreeCorpora &t);

};

QDataStream &operator<<(QDataStream &ds, const TreeCorpora &t);
QDataStream &operator>>(QDataStream &ds, TreeCorpora &t);

#endif // TREECORPORA_H
