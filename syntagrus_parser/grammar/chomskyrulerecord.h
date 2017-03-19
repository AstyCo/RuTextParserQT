#ifndef CHOMSKYRULERECORD_H
#define CHOMSKYRULERECORD_H

#include "internal/base-types.h"

struct ChomskyRuleRecord
{
    bool         isRightRule    : 1;
    linkID       link           : 7;    // 7 bit should be enought for sinto id (67)
    featureID    dependRule     : 12;
    featureID    sourceRule     : 12;

    ChomskyRuleRecord() : link(127), dependRule(-1), sourceRule(-1) {}

    ChomskyRuleRecord(const linkID &sintoID,
               const featureID &dependRuleID,
               const featureID &srcRuleID,
               const bool &right = true)
        :   isRightRule(right), link(sintoID),
          dependRule(dependRuleID), sourceRule(srcRuleID) {}

    featureID leftID() const { return isRightRule ? sourceRule : dependRule;}
    featureID rightID() const { return isRightRule ? dependRule : sourceRule;}
};

inline QDataStream &operator<<(QDataStream &ds, const ChomskyRuleRecord &s)
{
    ds << s.isRightRule;
    ds << s.link;
    ds << s.dependRule;
    ds << s.sourceRule;

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

    s.isRightRule = right;
    s.link = link;
    s.dependRule = dependRule;
    s.sourceRule = sourceRule;

    return ds;
}


#endif // CHOMSKYRULERECORD_H
