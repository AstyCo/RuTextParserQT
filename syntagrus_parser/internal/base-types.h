#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include "corpora_parsing/uniquevector.h"
#include "rutextparser_extensions/dumpable.h"

#include <QtCore>

// 8-bit unsigned integer.
typedef qint8 linkID;      // ( 0-67  syntagrus 2012.)
#define INVALID_LINK (static_cast<linkID>(127))

// 16-bit integer.
typedef qint16 ruleID;
// 16-bit integer.
typedef qint16 featureID;   // ( 0-535 syntagrus2012.)



struct FeatureMapper: public UniqueVector<featureID, QString>, public Dumpable
{
    FeatureMapper() : Dumpable("feature_mapper.dump") {}

    EXT_DECLARE_SERIALIZATION
};

inline QDataStream &operator<<(QDataStream &ds, const FeatureMapper &s) {
    const UniqueVector<featureID, QString>&uv = s;
    ds << uv;

    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, FeatureMapper &s) {
    UniqueVector<featureID, QString>&uv = s;
    ds >> uv;

    return ds;
}

struct LinkMapper: public UniqueVector<linkID, QString>, public Dumpable
{
    LinkMapper() : UniqueVector<linkID, QString>(INVALID_LINK), Dumpable("link_mapper.dump") {}

    EXT_DECLARE_SERIALIZATION
};

inline QDataStream &operator<<(QDataStream &ds, const LinkMapper &s) {
    const UniqueVector<linkID, QString>&uv = s;
    ds << uv;

    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, LinkMapper &s) {
    UniqueVector<linkID, QString>&uv = s;
    ds >> uv;

    return ds;
}


#endif // BASE_TYPES_H
