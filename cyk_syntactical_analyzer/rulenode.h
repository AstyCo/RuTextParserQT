#ifndef RULENODE_H
#define RULENODE_H

#include "cyk_syntactical_analyzer_global.h"
#include "syntagrus_parser/internal/base-types.h"
#include "syntagrus_parser/grammar/scoredrules.h"

#include <QList>

class CNFGrammar;
struct RuleNode;

struct CYK_SYNTACTICAL_ANALYZERSHARED_EXPORT RuleLink
{
    ruleID id;
    QSharedPointer<RuleNode> node;


    RuleLink(ruleID rid, QSharedPointer<RuleNode> ruleNode);
    bool operator==(const RuleLink &other) const;

    bool operator<(const RuleLink &other) const { return id < other.id;}
    bool operator>(const RuleLink &other) const { return !(*this < other);}
//    const RuleLink &operator=(const RuleLink &other);
};


class SimpleRuleNode;

struct CYK_SYNTACTICAL_ANALYZERSHARED_EXPORT RuleNode : public Scored
{
//    static long callgrind_debug;
    quint8 _delta;
    QList<RuleLink> _rules;
public:

    QString toString(const QVector<ScoredChomskyRuleRecord> &ruleByID,
                         const FeatureMapper &fmapper, const LinkMapper &lmapper,
                         int tab = 0) const;

    const RuleNode&operator=(const RuleNode &other);
    RuleNode(const RuleNode &other);
    RuleNode(qreal scoreValue = 0, quint8 delta = 0);
    RuleNode(ruleID rid, QSharedPointer<RuleNode> child, qreal scoreValue = 0, quint8 delta = 0);
    ~RuleNode();


    const SimpleRuleNode *grammarGetNodeWidth(const featureID &fid,  const CNFGrammar &grammar) const;
    qreal calcProb(ruleID prid, const CNFGrammar &grammar,
                   const FeatureMapper &fmapper, const LinkMapper &lmapper) const;

    qreal calcProb2(ruleID last, const CNFGrammar &grammar,
                    const FeatureMapper &fmapper, const LinkMapper &lmapper) const;
    quint8 &delta() { return _delta;}
    const quint8 &delta() const { return _delta;}

    const QList<RuleLink> &rules() const { return _rules;}
    void append(const RuleLink &link);
    void append(const QList<RuleLink> &links);


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

    const QList<QPair<featureID, linkID> > toLAS(const CNFGrammar &grammar) const;
    const QList<featureID> toUAS(const CNFGrammar &grammar) const;

private:
    const QPair<QList<QPair<featureID, linkID> >,
        QList<QPair<featureID, linkID> > > toLASH(const CNFGrammar &grammar) const;
};

typedef QList<RuleNode> ListRuleNode;


#endif // RULENODE_H
