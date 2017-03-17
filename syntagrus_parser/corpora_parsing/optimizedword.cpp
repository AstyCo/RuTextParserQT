#include "optimizedword.h"
#include "featuremapper.h"

#include <QHash>

QDataStream &operator<<(QDataStream &ds, const OptimizedWord &w)
{
    Q_ASSERT(w.sintRels.size() == w.words.size());

    ds << w.dom;
    ds << w.id;
    ds << w.feature;

    ds << w.sintRels;
    ds << w.words;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, OptimizedWord &w)
{
    ds >> w.dom;
    ds >> w.id;
    ds >> w.feature;

    ds >> w.sintRels;
    ds >> w.words;

    Q_ASSERT(w.sintRels.size() == w.words.size());
    return ds;
}

OptimizedWord OptimizedWord::fromWord(const WordInCorpora &word,
                                      const FeatureMapper &mapper)
{
    OptimizedWord res;
    res.dom = word.dom();
    res.id = word.index();
    Q_ASSERT(mapper.featureId(word.feature()) != -1);
    res.feature = mapper.featureId(word.feature());

    res.sintRels.append(word.links());
    res.words.append(word.word());

    return res;
}

const FeatureId &OptimizedWord::getFeature() const
{
    return feature;
}

void OptimizedWord::append(const WordInCorpora &word)
{
    Q_ASSERT(id == word.index());

    sintRels.append(word.links());
    words.append(word.word());
}

bool OptimizedWord::operator==(const OptimizedWord &w) const
{
    return (id == w.id) && (feature == w.feature);
}

QDebug operator<<(QDebug d, const OptimizedWord &w)
{
    if (!w.words.isEmpty())
        d << w.words.first();

    return d;
}
