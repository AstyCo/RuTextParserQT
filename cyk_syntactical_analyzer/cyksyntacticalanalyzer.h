#ifndef CYKANALYZER_H
#define CYKANALYZER_H

#include "cyk_syntactical_analyzer_global.h"
#include "grammar/cnfgrammar.h"
#include "cykmatrix.h"

#include <QList>

//typedef NonterminalList* NonterminalRow;
//typedef NonterminalRow* CYKMatrix;

typedef CYKMatrix *CYKMatrixPtr;
typedef QVector<CYKMatrixPtr> CYKMatrixVector;

typedef QList<QStringList> AmbigiousStringVector;
typedef QList<QStringList> Sentences;

typedef QList<QString> FeatureVector;

class CYK_SYNTACTICAL_ANALYZERSHARED_EXPORT CYKSyntacticalAnalyzer
{
public:
    CYKSyntacticalAnalyzer();

    static Sentences analyze(const AmbigiousStringVector &fv, const CNFGrammar &grammar);

private:
//    CYKMatrix allocateMatrix(int n); ///< allocates N x N CYK (lower triangular matrix)
//    void deallocateMatrix(const CYKMatrix &matrix, int size);

    static CYKMatrixVector *getSubmatrixVector(const AmbigiousStringVector &fv,
                                        int from, const CYKMatrixVector *lastEvaluated,
                                        const CNFGrammar &grammar);


};

#endif // CYKANALYZER_H
