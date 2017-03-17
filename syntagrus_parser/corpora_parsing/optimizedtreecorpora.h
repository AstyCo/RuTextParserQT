#ifndef OPTIMIZEDTREECORPORA_H
#define OPTIMIZEDTREECORPORA_H

#include "featuremapper.h"
#include "treecorpora.h"
#include "optimizedsentence.h"


class SYNTAGRUS_PARSERSHARED_EXPORT OptimizedTreeCorpora
{
    QString _dumpFilename;
    FeatureMapper _featureMapper;
    QMultiHash<int, OptimizedSentence> _multihashSentences;
public:
    OptimizedTreeCorpora();
    explicit OptimizedTreeCorpora(const TreeCorpora &tree);

    const QMultiHash<int, OptimizedSentence> &multihashSentences() const { return _multihashSentences;}
    const FeatureMapper &featureMapper() const { return _featureMapper;}

    void serialize() const;
    void deserialize();

    void clear();

    friend QDataStream &operator<<(QDataStream &ds, const OptimizedTreeCorpora &s);
    friend QDataStream &operator>>(QDataStream &ds, OptimizedTreeCorpora &s);

private:
    void initDumpFilename();
};

QDataStream &operator<<(QDataStream &ds, const OptimizedTreeCorpora &s);
QDataStream &operator>>(QDataStream &ds, OptimizedTreeCorpora &s);

#endif // OPTIMIZEDTREECORPORA_H
