#ifndef CHOMSKYRULERECORD_H
#define CHOMSKYRULERECORD_H

#include "internal/base-types.h"

struct ChomskyRuleRecord
{
    bool         isRightRule    : 1;
    linkID       link           : 7;    // 7 bit should be enought for sinto id (67)
    featureID    dependRule     : 12;
    featureID    sourceRule     : 12;

    ChomskyRuleRecord() : link(-1), dependRule(-1), sourceRule(-1) {}

    ChomskyRuleRecord(const linkID &sintoID,
               const featureID &dependRuleID,
               const featureID &srcRuleID,
               const bool &right = true)
        :   isRightRule(right), link(sintoID),
          dependRule(dependRuleID), sourceRule(srcRuleID) {}

    featureID leftID() const { return isRightRule ? sourceRule : dependRule;}
    featureID rightID() const { return isRightRule ? dependRule : sourceRule;}
};

#endif // CHOMSKYRULERECORD_H
