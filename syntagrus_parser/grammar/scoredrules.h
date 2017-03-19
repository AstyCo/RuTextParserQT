#ifndef SCOREDRULES_H
#define SCOREDRULES_H

#include "syntagrus_parser_global.h"
#include "chomskyrulerecord.h"

struct Scored
{
    qreal score;

    Scored(qreal initScore = 0) : score(initScore) {}
    void increaseScore() { ++score;}
};

SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const Scored &s);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, Scored &s);


struct ScoredChomskyRuleRecord : public Scored
{
    ChomskyRuleRecord rule;

    ScoredChomskyRuleRecord() : Scored() {}
    ScoredChomskyRuleRecord(const ChomskyRuleRecord &ch) : Scored(1), rule(ch) {}
};

SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const ScoredChomskyRuleRecord &s);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, ScoredChomskyRuleRecord &s);


struct ScoredRuleID : public Scored
{
    ruleID id;

    ScoredRuleID() : Scored(), id(-1) {}
    ScoredRuleID(const ruleID &rid) : Scored(1), id(rid) {}

};

SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const ScoredRuleID &s);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, ScoredRuleID &s);


typedef QList<ScoredRuleID> ListScoredRuleID;

typedef QList<ruleID> ListRuleID;
struct ScoredListRuleID : public Scored
{
    ListRuleID list;

    ScoredListRuleID() : Scored() {}
    ScoredListRuleID(const ListRuleID &ids) : Scored(1), list(ids) {}

};
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const ScoredListRuleID &s);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, ScoredListRuleID &s);


typedef QList<ScoredListRuleID> ListScoredListRuleID;



#endif // SCOREDRULES_H
