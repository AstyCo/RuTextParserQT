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
    static long callgrind_debug;
    quint8 _delta;
    QList<RuleLink> _rules;
public:

    const RuleNode&operator=(const RuleNode &other);
    RuleNode(const RuleNode &other);
    RuleNode(qreal scoreValue = 0, quint8 delta = 0);
    RuleNode(ruleID rid, QSharedPointer<RuleNode> child, qreal scoreValue = 0, quint8 delta = 0);
    ~RuleNode();

    quint8 &delta() { return _delta;}
    const quint8 &delta() const { return _delta;}

    const QList<RuleLink> &rules() const { return _rules;}
    bool insert(const RuleLink &link);


    bool isLeaf() const { return _rules.isEmpty();}
    bool operator==(const RuleNode &other) const {
        if (_rules.isEmpty() && other._rules.isEmpty())
            return true;
        return _rules == other._rules;
    }

    bool operator<(const RuleNode &other) const {
        if (other._rules.isEmpty())
            return false;

        return _rules < other._rules;
    }
};

typedef QList<RuleNode> ListRuleNode;


#endif // RULENODE_H
