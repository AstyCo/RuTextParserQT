#ifndef OPTIMIZEDWORD_H
#define OPTIMIZEDWORD_H

#include "wordincorpora.h"

typedef qint16 FeatureId;


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

    /// static
    static OptimizedWord fromWord(const WordInCorpora &word, const QHash<QString, FeatureId> &hashFeatures);
};

QDataStream &operator<<(QDataStream &ds, const OptimizedWord &w);
QDataStream &operator>>(QDataStream &ds, OptimizedWord &w);

#endif // OPTIMIZEDWORD_H
