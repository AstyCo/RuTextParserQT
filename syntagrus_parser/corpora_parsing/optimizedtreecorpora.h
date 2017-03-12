#ifndef OPTIMIZEDTREECORPORA_H
#define OPTIMIZEDTREECORPORA_H

#include "treecorpora.h"
#include "optimizedsentence.h"


struct SYNTAGRUS_PARSERSHARED_EXPORT FeatureMapper
{
    QVector<QString> features;
    QHash<QString, FeatureId> hashFeatures;
};

class SYNTAGRUS_PARSERSHARED_EXPORT OptimizedTreeCorpora
{
    FeatureMapper _featureMapper;
    QMultiHash<int, OptimizedSentence> _multihashSentences;
public:
    explicit OptimizedTreeCorpora(const TreeCorpora &tree);

    const QMultiHash<int, OptimizedSentence> &multihashSentences() const { return _multihashSentences;}
    const FeatureMapper &featureMapper() const { return _featureMapper;}
};

#endif // OPTIMIZEDTREECORPORA_H
