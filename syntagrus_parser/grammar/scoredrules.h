#ifndef SCOREDRULES_H
#define SCOREDRULES_H

#include "syntagrus_parser_global.h"
#include "chomskyrulerecord.h"

struct Scored
{
    qreal score;

    Scored(qreal initScore = 0) : score(initScore) {}
    void increaseScore(qreal val = 1) { score+=val;}
};

SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const Scored &s);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, Scored &s);


struct ScoredChomskyRuleRecord : public Scored
{
    ChomskyRuleRecord rule;

    ScoredChomskyRuleRecord() : Scored() {}
    ScoredChomskyRuleRecord(const ChomskyRuleRecord &ch) : Scored(1), rule(ch) {}

    QString toString(const FeatureMapper &fmapper, const LinkMapper &lmapper) const { return rule.toString(fmapper, lmapper) + QString(" (score:%1)").arg(score);}
};

inline bool operator<(const ScoredChomskyRuleRecord &lhs, const ScoredChomskyRuleRecord &rhs) {
    return lhs.rule < rhs.rule;
}

SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const ScoredChomskyRuleRecord &s);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, ScoredChomskyRuleRecord &s);


struct ScoredRuleID : public Scored
{
    ruleID id;

    ScoredRuleID() : Scored(), id(-1) {}
    ScoredRuleID(const ruleID &rid) : Scored(1), id(rid) {}

};

inline bool operator<(const ScoredRuleID &lhs, const ScoredRuleID &rhs) {
    return lhs.id < rhs.id;
}

SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const ScoredRuleID &s);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, ScoredRuleID &s);


typedef QList<ScoredRuleID> ListScoredRuleID;

typedef QSet<ruleID> SetRuleID;
typedef QList<ruleID> ListRuleID;

struct ScoredSetRuleID : public Scored
{
    SetRuleID set;

    ScoredSetRuleID(qreal score = 0) : Scored(score) {}
    ScoredSetRuleID(const SetRuleID &ids, qreal score = 1) : Scored(score), set(ids) {}

//    bool operator==(const ScoredSetRuleID &other) const { return set == other.set;}
};

struct ScoredListRuleID : public Scored
{
    ListRuleID list;

    ScoredListRuleID(qreal score = 0) : Scored(score) {}
    ScoredListRuleID(const ListRuleID &ids, qreal score = 1) : Scored(score), list(ids) {}

    bool operator==(const ScoredListRuleID &other) const { return list == other.list;}
};

SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const ScoredSetRuleID &s);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, ScoredSetRuleID &s);

typedef QList<ScoredListRuleID> ListScoredListRuleID;
typedef QSet<ScoredSetRuleID> SetScoredSetRuleID;



#endif // SCOREDRULES_H
