#include "optimizedword.h"

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

OptimizedWord OptimizedWord::fromWord(const WordInCorpora &word, const QHash<QString, FeatureId> &hashFeatures)
{
    OptimizedWord res;
    res.dom = word.dom();
    res.id = word.index();
    Q_ASSERT(hashFeatures.contains(word.feature()));
    res.feature = hashFeatures[word.feature()];

    res.sintRels.append(word.links());
    res.words.append(word.word());

    return res;
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
