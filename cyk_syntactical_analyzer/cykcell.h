#ifndef CYKCELL_H
#define CYKCELL_H

#include "syntagrus_parser/internal/base-types.h"
#include "rulenode.h"

struct CYKCellRecord
{
    featureID fid;
    QList<RuleNode> _topRules;

    CYKCellRecord(const featureID &initFid = -1) : fid(initFid) {}
};

typedef QList<CYKCellRecord> CYKCell;

#endif // CYKCELL_H
