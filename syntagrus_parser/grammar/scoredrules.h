#ifndef SCOREDRULES_H
#define SCOREDRULES_H

#include "chomskyrulerecord.h"

struct Scored
{
    qreal score;

    Scored(qreal initScore = 0) : score(initScore) {}
    void increaseScore() { ++score;}
};


struct ScoredChomskyRuleRecord : public Scored
{
    ChomskyRuleRecord rule;

    ScoredChomskyRuleRecord() : Scored() {}
    ScoredChomskyRuleRecord(const ChomskyRuleRecord &ch) : Scored(1), rule(ch) {}
};

struct ScoredRuleID : public Scored
{
    ruleID id;

    ScoredRuleID() : Scored(), id(-1) {}
    ScoredRuleID(const ruleID &rid) : Scored(1), id(rid) {}

};

typedef QList<ScoredRuleID> ListScoredRuleID;

typedef QList<ruleID> ListRuleID;
struct ScoredListRuleID : public Scored
{
    ListRuleID list;

    ScoredListRuleID() : Scored() {}
    ScoredListRuleID(const ListRuleID &ids) : Scored(1), list(ids) {}

};
typedef QList<ScoredListRuleID> ListScoredListRuleID;



#endif // SCOREDRULES_H
