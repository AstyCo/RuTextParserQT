#ifndef CYKANALYZER_H
#define CYKANALYZER_H

#include "cyk_syntactical_analyzer_global.h"
#include "grammar/cnfgrammar.h"
#include "cykmatrix.h"
#include "syntagrus_parser/corpora_parsing/uniquevector.h"

#include <QList>


typedef CYKMatrix *CYKMatrixPtr;
typedef QVector<CYKMatrixPtr> CYKMatrixVector;

typedef QList<QStringList> AmbigiousStringVector;
typedef QList<QStringList> Sentences;

typedef QList<QString> FeatureVector;

class CYK_SYNTACTICAL_ANALYZERSHARED_EXPORT CYKSyntacticalAnalyzer
{
    QString _dumpFilename;

    FeatureMapper _fmapper;
    LinkMapper _lmapper;

public:
    CYKSyntacticalAnalyzer(const QString &filename = QString());   ///< for deserialization
    CYKSyntacticalAnalyzer(const FeatureMapper &fm, const LinkMapper &lm);

    QList<RuleNode> analyze(const AmbigiousStringVector &fv, const CNFGrammar &grammar);
    QList<RuleNode> analyze(const QStringList &v, const CNFGrammar &grammar);

private:
    void loadFromDump(const QString &filename);

    CYKMatrixVector *getSubmatrixVector(const AmbigiousStringVector &fv,
                                 int from, const CYKMatrixVector *lastEvaluated,
                                 const CNFGrammar &grammar);

    void fillLastRow(const AmbigiousStringVector &fv, CYKMatrix &matrix);

};

#endif // CYKANALYZER_H
