#ifndef RULERECORD_H
#define RULERECORD_H

//#include "syntagrus_parser_global.h"
#include "internal/base-types.h"

struct RuleDepend
{
    bool isRight    : 1;
    linkID link     : 7;

    featureID terminal;

    RuleDepend(const linkID &ruleLinkID, const featureID &ruleTerminalID, const bool &right = true)
        : isRight(right), link(ruleLinkID), terminal(ruleTerminalID) {}
};

class RuleRecord
{
    featureID    _sourceRule;
    QList<RuleDepend> _depends;
public:
    RuleRecord(const featureID &srcRuleID = -1)
        :   _sourceRule(srcRuleID) {}

    void setSource(const featureID &sourceRuleID) { _sourceRule = sourceRuleID;}
    void append(const linkID &ruleLinkID, const featureID &ruleTerminalID, const bool &right = true) {
        _depends.append(RuleDepend(ruleLinkID, ruleTerminalID, right));
    }
    void clear() { _sourceRule=-1; _depends.clear();}
    featureID &sourceRule() { return _sourceRule;}
    const featureID &sourceRule() const { return _sourceRule;}

    QList<RuleDepend> &depends() { return _depends;}
    const QList<RuleDepend> &depends() const { return _depends;}


};

#endif // RULERECORD_H
