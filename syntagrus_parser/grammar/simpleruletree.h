#ifndef SIMPLERULETREE_H
#define SIMPLERULETREE_H

#include "syntagrus_parser_global.h"
#include "internal/base-types.h"
#include "scoredrules.h"

#include <QMap>

struct SYNTAGRUS_PARSERSHARED_EXPORT SimpleRuleNode : public Scored
{
//    ruleID idThis;
    QMap<ruleID, SimpleRuleNode> node;
    Scored rootScore;

    SimpleRuleNode(qreal score = 0, qreal initRootScore = 0) : Scored(score), rootScore(initRootScore) {}

    int totalScore() const;
    int totalRoortScore() const;
    void clear();

    friend QDataStream &operator<<(QDataStream &ds, const SimpleRuleNode &n);
    friend QDataStream &operator>>(QDataStream &ds, SimpleRuleNode &n);
};

SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const SimpleRuleNode &n);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, SimpleRuleNode &n);

class SYNTAGRUS_PARSERSHARED_EXPORT SimpleRuleTree
{
    SimpleRuleNode _root;
public:
    SimpleRuleTree();

    void insert(const QList<ruleID> &list, bool rootRule);
    bool contains(const QList<ruleID> &list) const;
    const SimpleRuleNode *node(const QList<ruleID> &list) const;
    SimpleRuleNode *node(const QList<ruleID> &list);

    int size() const { return totalScore();}
    int totalScore() const;
    int totalRootScore() const;
    void clear() { _root.clear();}

    friend QDataStream &operator<<(QDataStream &ds, const SimpleRuleTree &tree);
    friend QDataStream &operator>>(QDataStream &ds, SimpleRuleTree &tree);
};

SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const SimpleRuleTree &tree);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, SimpleRuleTree &tree);

#endif // SIMPLERULETREE_H
