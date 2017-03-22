#ifndef CHOMSKYRULERECORD_H
#define CHOMSKYRULERECORD_H

#include "internal/base-types.h"

struct ChomskyRuleRecord
{
    bool         _isRightRule    : 1;
    linkID       _linkID         : 7;    // 7 bit should be enought to deal sinto id (67)
    featureID    _dependFID      : 12;
    featureID    _sourceFID      : 12;

    ChomskyRuleRecord() : _linkID(127), _dependFID(-1), _sourceFID(-1) {}

    ChomskyRuleRecord(const linkID &sintoID,
               const featureID &dependRuleID,
               const featureID &srcRuleID,
               const bool &right = true)
        :   _isRightRule(right), _linkID(sintoID),
          _dependFID(dependRuleID), _sourceFID(srcRuleID) {}

    featureID leftID() const { return _isRightRule ? _sourceFID : _dependFID;}
    featureID rightID() const { return _isRightRule ? _dependFID : _sourceFID;}
};

inline bool operator<(const ChomskyRuleRecord &lhs, const ChomskyRuleRecord &rhs) {
    return lhs._sourceFID < rhs._sourceFID;
}

inline QDataStream &operator<<(QDataStream &ds, const ChomskyRuleRecord &s)
{
    ds << s._isRightRule;
    ds << s._linkID;
    ds << s._dependFID;
    ds << s._sourceFID;

    return ds;
}
inline QDataStream &operator>>(QDataStream &ds, ChomskyRuleRecord &s)
{
    bool right;
    linkID link;
    featureID dependRule;
    featureID sourceRule;


    ds >> right;
    ds >> link;
    ds >> dependRule;
    ds >> sourceRule;

    s._isRightRule = right;
    s._linkID = link;
    s._dependFID = dependRule;
    s._sourceFID = sourceRule;

    return ds;
}


#endif // CHOMSKYRULERECORD_H
