#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include "corpora_parsing/uniquevector.h"

#include <QtCore>

// 8-bit unsigned integer.
typedef quint8 linkID;      // ( 0-67  syntagrus 2012.)
#define INVALID_LINK (static_cast<linkID>(127))

// 16-bit integer.
typedef qint16 ruleID;
// 16-bit integer.
typedef qint16 featureID;   // ( 0-535 syntagrus2012.)

typedef UniqueVector<featureID, QString> FeatureMapper;
typedef UniqueVector<linkID, QString> LinkMapper;


#endif // BASE_TYPES_H
