#include "optimizedtreecorpora.h"

OptimizedTreeCorpora::OptimizedTreeCorpora(const TreeCorpora &tree)
{
    const QList<QString> &treeFeatures = tree.features();

    _featureMapper.features.resize(treeFeatures.size());
    for (int i=0; i < treeFeatures.size(); ++i) {
        _featureMapper.features[i] = treeFeatures[i];
        _featureMapper.hashFeatures.insert(treeFeatures[i], i);
    }
    QMultiHash<int, SentenceInCorpora>::const_iterator it(tree.sentencesBySize().constBegin());
    while( it != tree.sentencesBySize().constEnd()) {
        OptimizedSentence optimizedSentence(it.value(), _featureMapper.hashFeatures);
        QMultiHash<int, OptimizedSentence>::iterator insertedSentence
                = _multihashSentences.find(it.key(), optimizedSentence);
        if (insertedSentence != _multihashSentences.end()) {
            insertedSentence.value().append(it.value());
        }
        else {
            _multihashSentences.insert(it.key(), optimizedSentence);
        }

        it++;
    }

}
