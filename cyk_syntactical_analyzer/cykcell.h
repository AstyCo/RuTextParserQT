#ifndef CYKCELL_H
#define CYKCELL_H

#include "syntagrus_parser/internal/base-types.h"
#include "rulenode.h"
#include "syntagrus_parser/grammar/scoredrules.h"

#include <QSharedPointer>

//typedef QMultiMap<featureID, RuleNode> CYKCell;

typedef QMultiMap<featureID, QSharedPointer<RuleNode> > FeatureMultiMap;

//struct CYK_SYNTACTICAL_ANALYZERSHARED_EXPORT CYKRuleNode
//{
//    QMap<ruleID, QSharedPointer<CYKRuleNode> > childs;
//    QList<QSharedPointer<CYKRuleNode> > value;

//    CYKRuleNode() : {}
//};

//class CYK_SYNTACTICAL_ANALYZERSHARED_EXPORT CYKRuleTree
//{
//    CYKRuleNode _root;
//public:
//    CYKRuleTree();

//    void insert(const QList<ruleID> &list, bool rootRule);
//    bool contains(const QList<ruleID> &list) const;
//    const CYKRuleNode *node(const QList<ruleID> &list) const;
//    CYKRuleNode *node(const QList<ruleID> &list);
//};




//class RulesNode
//{
//    QList<*RulesNode> childs;

//};

class RulesTree
{

};

struct CYKCell: public QMultiMap<featureID, QSharedPointer<RuleNode> >
{
    bool contains(const featureID &key, const QSharedPointer<RuleNode> &value) const {
        CYKCell::const_iterator i(this->FeatureMultiMap::find(key));
        while (i!=this->constEnd() && i.key() == key) {
            if (*i.value() == *value)
                return true;
            ++i;
        }
        return false;
    }

    CYKCell::iterator find(const featureID &key, const QSharedPointer<RuleNode> &value) {
        CYKCell::iterator i(this->FeatureMultiMap::find(key));
        CYKCell::iterator end(this->end());
        while (i != end && !qMapLessThanKey<featureID>(key, i.key())) {
            if (*i.value() == *value)
                return i;
            ++i;
        }
        return end;
    }
};

//struct ScoredFeatureID: public Scored
//{
//    featureID fid;

//    ScoredFeatureID(qreal scoreVal = 0) : Scored(scoreVal) {}
//};


typedef CYKCell::iterator CYKCellIterator;
typedef CYKCell::const_iterator CYKCellConstIterator;

#endif // CYKCELL_H
