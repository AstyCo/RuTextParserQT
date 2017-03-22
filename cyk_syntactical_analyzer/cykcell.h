#ifndef CYKCELL_H
#define CYKCELL_H

#include "syntagrus_parser/internal/base-types.h"
#include "rulenode.h"
#include "syntagrus_parser/grammar/scoredrules.h"

#include <QSharedPointer>

//typedef QMultiMap<featureID, RuleNode> CYKCell;

struct CYKCell: public QMultiMap<featureID, QSharedPointer<RuleNode> >
{

};

//struct ScoredFeatureID: public Scored
//{
//    featureID fid;

//    ScoredFeatureID(qreal scoreVal = 0) : Scored(scoreVal) {}
//};


typedef CYKCell::iterator CYKCellIterator;
typedef CYKCell::const_iterator CYKCellConstIterator;

#endif // CYKCELL_H
