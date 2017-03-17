#ifndef FEATUREMAPPER_H
#define FEATUREMAPPER_H

#include "syntagrus_parser_global.h"
#include "optimizedword.h"

#include <QVector>
#include <QHash>

class SYNTAGRUS_PARSERSHARED_EXPORT FeatureMapper
{
    QVector<QString> features;
    QHash<QString, FeatureId> hashFeatures;
public:

    void addFeature(const QString &feature);
    const QString &feature(const FeatureId &id) const;
    FeatureId featureId(const QString &feature) const;
    int size() const { return features.size();}

    void clear();

    friend QDataStream &operator<<(QDataStream &ds, const FeatureMapper &f);
    friend QDataStream &operator>>(QDataStream &ds, FeatureMapper &f);
};
QDataStream &operator<<(QDataStream &ds, const FeatureMapper &f);
QDataStream &operator>>(QDataStream &ds, FeatureMapper &f);

#endif // FEATUREMAPPER_H
