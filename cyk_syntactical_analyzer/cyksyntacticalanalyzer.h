#ifndef CYKANALYZER_H
#define CYKANALYZER_H

#include "cyk_syntactical_analyzer_global.h"
#include "grammar/cnfgrammar.h"
#include "cykmatrix.h"
#include "syntagrus_parser/corpora_parsing/uniquevector.h"

#include <QList>
#include <QElapsedTimer>


typedef QList<QStringList> AmbigiousStringVector;
typedef QList<QStringList> Sentences;

typedef QList<QString> FeatureVector;

class CYK_SYNTACTICAL_ANALYZERSHARED_EXPORT CYKSyntacticalAnalyzer
{
    QElapsedTimer timer;
    qint64 maxTime;

    FeatureMapper _fmapper;
    LinkMapper _lmapper;
public:
    CYKSyntacticalAnalyzer(const QString &fmapper = QString(), const QString &lmapper = QString());   ///< for deserialization
    CYKSyntacticalAnalyzer(const FeatureMapper &fm, const LinkMapper &lm);

    QList<QSharedPointer<RuleNode> > analyze(const AmbigiousStringVector &fv, const CNFGrammar &grammar, QString *error = NULL);
    QList<QSharedPointer<RuleNode> > analyze(const QStringList &v, const CNFGrammar &grammar, QString *error = NULL);

    void setMaxTime(const qint64 &value);

private:
    //    ListRuleID produceCYKSequence(const RuleNode &rn, const CNFGrammar &grammar);
    void fillLastRow(const AmbigiousStringVector &fv, CYKMatrix &matrix);
    bool calcCell(CYKMatrix &matrix, const int &i, const int &j, const CNFGrammar &grammar, QString *error);
    void addRecord(CYKCell &cell,
                   const QSharedPointer<RuleNode> &l,
                   const QSharedPointer<RuleNode> &r,
                   const ListRuleID &scoredRuleIDs, const CNFGrammar &grammar);

    bool grammarContainsRuleDepth(ruleID rid, const RuleNode *dep, const CNFGrammar &grammar) const;
    const SimpleRuleNode *grammarGetNodeWidth(const featureID &fid, const RuleNode &rn, const CNFGrammar &grammar) const;
    const SimpleRuleNode *grammarFinalScore(const featureID &fid, const RuleNode &rn, const CNFGrammar &grammar) const;

    long _debugCellTotal, _debugCellRefusedWidth, _debugCellRefusedDepth, _debugCellRefusedLogical;
};

#endif // CYKANALYZER_H
