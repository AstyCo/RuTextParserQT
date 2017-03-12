#ifndef OPTIMIZEDSENTENCE_H
#define OPTIMIZEDSENTENCE_H

#include "syntagrus_parser_global.h"
#include "optimizedword.h"
#include "sentenceincorpora.h"

#include <QVector>

class SYNTAGRUS_PARSERSHARED_EXPORT OptimizedSentence
{
    QVector<OptimizedWord> _words;

public:
    OptimizedSentence();
    OptimizedSentence(const SentenceInCorpora &sentence,
                      const QHash<QString, FeatureId> &hashFeatures);

    QVector<OptimizedWord> &words() { return _words;}
    void append(const SentenceInCorpora &s);
    bool operator==(const OptimizedSentence &os);



    /// static
    static OptimizedSentence fromSentence(const SentenceInCorpora &sentence,
                                          const QHash<QString, FeatureId> &hashFeatures);
};

#endif // OPTIMIZEDSENTENCE_H
