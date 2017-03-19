#ifndef OPTIMIZEDWORD_H
#define OPTIMIZEDWORD_H

#include "wordincorpora.h"
#include "corpora_parsing/uniquevector.h"
#include "internal/base-types.h"

#include <QDebug>

class IntMapper;

struct SYNTAGRUS_PARSERSHARED_EXPORT OptimizedWord
{
    WordIndex dom;
    WordIndex id;
    featureID feature;
    QList<SintRelList> sintRels;
    QList<QString> words;

public:
    OptimizedWord() : dom(-1), id(-1), feature(-1) {}

    void append(const WordInCorpora &word);
    /// operators
    bool operator==(const OptimizedWord&w) const;
    int size() const { Q_ASSERT(sintRels.size() == words.size()); return words.size();}

    const featureID &getFeature() const;

    /// static
    static OptimizedWord fromWord(const WordInCorpora &word,
                                  const UniqueVector<featureID, QString> &mapper);

    friend QDebug operator<<(QDebug d, const OptimizedWord &w);
};

QDataStream &operator<<(QDataStream &ds, const OptimizedWord &w);
QDataStream &operator>>(QDataStream &ds, OptimizedWord &w);

SYNTAGRUS_PARSERSHARED_EXPORT QDebug operator<<(QDebug d, const OptimizedWord &w);

#endif // OPTIMIZEDWORD_H
