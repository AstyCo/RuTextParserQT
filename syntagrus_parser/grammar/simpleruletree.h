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

    SimpleRuleNode(qreal score = 0) : Scored(score) {}

    int size() const;

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

    void insert(const QList<ruleID> &list);
    bool contains(const QList<ruleID> &list) const;
    const SimpleRuleNode *node(const QList<ruleID> &list) const;
    SimpleRuleNode *node(const QList<ruleID> &list);
    int size() const;

    friend QDataStream &operator<<(QDataStream &ds, const SimpleRuleTree &tree);
    friend QDataStream &operator>>(QDataStream &ds, SimpleRuleTree &tree);
};

SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const SimpleRuleTree &tree);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, SimpleRuleTree &tree);

#endif // SIMPLERULETREE_H
