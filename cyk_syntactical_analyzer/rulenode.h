#ifndef RULENODE_H
#define RULENODE_H

#include "syntagrus_parser/internal/base-types.h"
#include "syntagrus_parser/grammar/scoredrules.h"

#include <QList>


struct RuleNode;

struct RuleLink
{
    ruleID id;
    QSharedPointer<RuleNode> node;


    RuleLink(ruleID rid, QSharedPointer<RuleNode> ruleNode);
    bool operator==(const RuleLink &other) const;

    bool operator<(const RuleLink &other) const { return id < other.id;}
    bool operator>(const RuleLink &other) const { return !(*this < other);}
};

struct RuleNode : public Scored
{
    QList<RuleLink> rules;

    RuleNode(qreal scoreValue = 0);
    RuleNode(ruleID rid, QSharedPointer<RuleNode> child, qreal scoreValue = 0);


    bool isLeaf() const { return rules.isEmpty();}
    bool operator==(const RuleNode &other) const {
        if (rules.isEmpty() && other.rules.isEmpty())
            return true;
        return rules == other.rules;
    }

    bool operator<(const RuleNode &other) const {
        if (other.rules.isEmpty())
            return false;

        return rules < other.rules;
    }
};

typedef QList<RuleNode> ListRuleNode;


#endif // RULENODE_H
