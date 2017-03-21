#ifndef CYKANALYZER_H
#define CYKANALYZER_H

#include "cyk_syntactical_analyzer_global.h"
#include "grammar/cnfgrammar.h"
#include "cykmatrix.h"
#include "syntagrus_parser/corpora_parsing/uniquevector.h"

#include <QList>


typedef QList<QStringList> AmbigiousStringVector;
typedef QList<QStringList> Sentences;

typedef QList<QString> FeatureVector;

class CYK_SYNTACTICAL_ANALYZERSHARED_EXPORT CYKSyntacticalAnalyzer
{
    FeatureMapper _fmapper;
    LinkMapper _lmapper;

public:
    CYKSyntacticalAnalyzer(const QString &fmapper = QString(), const QString &lmapper = QString());   ///< for deserialization
    CYKSyntacticalAnalyzer(const FeatureMapper &fm, const LinkMapper &lm);

    QList<ListRuleNode> analyze(const AmbigiousStringVector &fv, const CNFGrammar &grammar);
    QList<ListRuleNode> analyze(const QStringList &v, const CNFGrammar &grammar);

private:
    void fillLastRow(const AmbigiousStringVector &fv, CYKMatrix &matrix);
    void calcCell(CYKMatrix &matrix, const int &i, const int &j, const CNFGrammar &grammar);

};

#endif // CYKANALYZER_H
