#ifndef SENTENCEINFO_H
#define SENTENCEINFO_H

#include "syntagrus_parser_global.h"

#include "wordincorpora.h"

#include <QList>
#include <QVector>
#include <QStringList>
#include <QDataStream>

#include <QDebug>

class SYNTAGRUS_PARSERSHARED_EXPORT SentenceInfo
{
    QVector<WordInCorpora> _vectorInfos;
public:
    SentenceInfo();

    inline const QVector<WordInCorpora> &vector() const { return _vectorInfos;}
    inline QVector<WordInCorpora> &vector() { return _vectorInfos;}

    const WordInCorpora &operator[](int i) const;
    WordInCorpora &operator[](int i);
    inline void append(const WordInCorpora &word) { _vectorInfos.append(word);}
    inline void clear() { _vectorInfos.clear();}
    inline void resize(int newSize) { _vectorInfos.resize(newSize);}
    inline int size() const { return _vectorInfos.size();}

    friend QDataStream &operator<<(QDataStream &ds, const SentenceInfo &s);
    friend QDataStream &operator>>(QDataStream &ds, SentenceInfo &s);

    QStringList wordList() const;
    QStringList featList() const;

};

QDataStream &operator<<(QDataStream &ds, const SentenceInfo &s);
QDataStream &operator>>(QDataStream &ds, SentenceInfo &s);

#endif // SENTENCEINFO_H
