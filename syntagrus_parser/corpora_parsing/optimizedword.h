#ifndef OPTIMIZEDWORD_H
#define OPTIMIZEDWORD_H

#include "wordincorpora.h"

#include <QDebug>

typedef qint16 FeatureId;

class FeatureMapper;

struct SYNTAGRUS_PARSERSHARED_EXPORT OptimizedWord
{
    WordIndex dom;
    WordIndex id;
    FeatureId feature;
    QList<SintRelList> sintRels;
    QList<QString> words;

public:
    OptimizedWord() : dom(-1), id(-1), feature(-1) {}

    void append(const WordInCorpora &word);
    /// operators
    bool operator==(const OptimizedWord&w) const;
    int size() const { Q_ASSERT(sintRels.size() == words.size()); return words.size();}

    const FeatureId &getFeature() const;

    /// static
    static OptimizedWord fromWord(const WordInCorpora &word,
                                  const FeatureMapper &mapper);

    friend QDebug operator<<(QDebug d, const OptimizedWord &w);
};

QDataStream &operator<<(QDataStream &ds, const OptimizedWord &w);
QDataStream &operator>>(QDataStream &ds, OptimizedWord &w);

SYNTAGRUS_PARSERSHARED_EXPORT QDebug operator<<(QDebug d, const OptimizedWord &w);

#endif // OPTIMIZEDWORD_H
