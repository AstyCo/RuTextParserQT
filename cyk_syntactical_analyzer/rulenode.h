#ifndef RULENODE_H
#define RULENODE_H

#include "syntagrus_parser/internal/base-types.h"

#include <QList>

struct RuleNode
{
    ruleID id;
    QList<RuleNode> childs;

    RuleNode(const ruleID &initID = -1) : id(initID) {}
};


#endif // RULENODE_H
