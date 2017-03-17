#include "featuremapper.h"


QDataStream &operator<<(QDataStream &ds, const FeatureMapper &f)
{
    ds << f.features;
    ds << f.hashFeatures;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, FeatureMapper &f)
{
    ds >> f.features;
    ds >> f.hashFeatures;

    return ds;
}

void FeatureMapper::addFeature(const QString &feature)
{
    features.append(feature);
    hashFeatures.insert(feature, features.size());  // last + 1
}

const QString &FeatureMapper::feature(const FeatureId &id) const
{
    Q_ASSERT(id > 0);
    return features[id - 1];
}

FeatureId FeatureMapper::featureId(const QString &feature) const
{
    if (!hashFeatures.contains(feature))
        return -1;
    return hashFeatures[feature];
}

void FeatureMapper::clear() {
    features.clear();
    hashFeatures.clear();
}
