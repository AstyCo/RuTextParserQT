#include "scoredrules.h"

QDataStream &operator<<(QDataStream &ds, const Scored &s) {
    ds << s.score;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, Scored &s)
{
    ds >> s.score;

    return ds;
}

QDataStream &operator<<(QDataStream &ds, const ScoredChomskyRuleRecord &s)
{
    ds << s.score;
    ds << s.rule;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, ScoredChomskyRuleRecord &s)
{
    ds >> s.score;
    ds >> s.rule;

    return ds;
}

QDataStream &operator<<(QDataStream &ds, const ScoredRuleID &s)
{
    ds << s.score;
    ds << s.id;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, ScoredRuleID &s)
{
    ds >> s.score;
    ds >> s.id;

    return ds;
}

QDataStream &operator<<(QDataStream &ds, const ScoredListRuleID &s)
{
    ds << s.score;
    ds << s.list;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, ScoredListRuleID &s)
{
    ds >> s.score;
    ds >> s.list;

    return ds;
}
