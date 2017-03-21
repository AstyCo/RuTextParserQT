#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include "corpora_parsing/uniquevector.h"
#include "rutextparser_extensions/dumpable.h"

#include <QtCore>

// 8-bit unsigned integer.
typedef quint8 linkID;      // ( 0-67  syntagrus 2012.)
#define INVALID_LINK (static_cast<linkID>(127))

// 16-bit integer.
typedef qint16 ruleID;
// 16-bit integer.
typedef qint16 featureID;   // ( 0-535 syntagrus2012.)

struct FeatureMapper: public UniqueVector<featureID, QString>, public Dumpable
{
    FeatureMapper() : Dumpable("feature_mapper.dump") {}
};

struct LinkMapper: public UniqueVector<featureID, QString>, public Dumpable
{
    LinkMapper() : Dumpable("link_mapper.dump") {}
};



#endif // BASE_TYPES_H
