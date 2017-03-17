#ifndef OPTIMIZEDSENTENCE_H
#define OPTIMIZEDSENTENCE_H

#include "syntagrus_parser_global.h"
#include "optimizedword.h"
#include "sentenceincorpora.h"

#include <QVector>

class FeatureMapper;

class SYNTAGRUS_PARSERSHARED_EXPORT OptimizedSentence
{
    QVector<OptimizedWord> _words;

public:
    OptimizedSentence();
    OptimizedSentence(const SentenceInCorpora &sentence,
                      const FeatureMapper &mapper);

    QVector<OptimizedWord> &words() { return _words;}
    const QVector<OptimizedWord> &words() const { return _words;}
    void append(const SentenceInCorpora &s);
    bool operator==(const OptimizedSentence &os);

    char *toKey() const;



    friend QDataStream &operator<<(QDataStream &ds, const OptimizedSentence &s);
    friend QDataStream &operator>>(QDataStream &ds, OptimizedSentence &s);
    /// static
    static OptimizedSentence fromSentence(const SentenceInCorpora &sentence,
                                          const QHash<QString, FeatureId> &hashFeatures);
};

QDataStream &operator<<(QDataStream &ds, const OptimizedSentence &s);
QDataStream &operator>>(QDataStream &ds, OptimizedSentence &s);

SYNTAGRUS_PARSERSHARED_EXPORT QDebug operator<<(QDebug d, const OptimizedSentence &s);

#endif // OPTIMIZEDSENTENCE_H
