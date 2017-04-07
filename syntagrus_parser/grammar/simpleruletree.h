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

    int totalCount() const;
    int totalScore() const;
    int totalRootScore() const;
    int totalRootCount() const;
    void clear();

    friend QDataStream &operator<<(QDataStream &ds, const SimpleRuleNode &n);
    friend QDataStream &operator>>(QDataStream &ds, SimpleRuleNode &n);
};

SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const SimpleRuleNode &n);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, SimpleRuleNode &n);

class SYNTAGRUS_PARSERSHARED_EXPORT SimpleRuleTree
{
    SimpleRuleNode _root;

    int _hashTotalScore, _hashTotalCount, _hashTotalRootScore, _hashTotalRootCount;
public:
    SimpleRuleTree();

    void insert(const QList<ruleID> &list, bool rootRule);
    bool contains(const QList<ruleID> &list) const;
    const SimpleRuleNode *node(const QList<ruleID> &list) const;
    SimpleRuleNode *node(const QList<ruleID> &list);

    int size() const { return totalScore();}

    void calcAll() {
        calcTotalScore();
        calcTotalCount();
        calcTotalRootScore();
        calcTotalRootCount();
    }

    void calcTotalScore() { _hashTotalScore = _root.totalScore();}
    void calcTotalCount() { _hashTotalCount = _root.totalCount();}
    void calcTotalRootScore() { _hashTotalRootScore = _root.totalRootScore();}
    void calcTotalRootCount() { _hashTotalRootCount = _root.totalRootCount();}

    int totalScore() const
    {
//        if (_hashTotalScore == -1)
//            _hashTotalScore = _root.totalScore();
        return _hashTotalScore;
    }
    int totalCount() const {
//        if (_hashTotalCount == -1)
//            _hashTotalCount = _root.totalCount();
        return _hashTotalCount;
    }
    int totalRootScore() const
    {
//        if (_hashTotalRootScore == -1)
//            _hashTotalRootScore = _root.totalRootScore();
        return _hashTotalRootScore;
    }
    int totalRootCount() const
    {
//        if (_hashTotalRootCount == -1)
//            _hashTotalRootCount = _root.totalRootCount();
        return _hashTotalRootCount;
    }
    void clear() { _root.clear();}

    friend QDataStream &operator<<(QDataStream &ds, const SimpleRuleTree &tree);
    friend QDataStream &operator>>(QDataStream &ds, SimpleRuleTree &tree);
};

SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator<<(QDataStream &ds, const SimpleRuleTree &tree);
SYNTAGRUS_PARSERSHARED_EXPORT QDataStream &operator>>(QDataStream &ds, SimpleRuleTree &tree);

#endif // SIMPLERULETREE_H
