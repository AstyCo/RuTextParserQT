#include "cyksyntacticalanalyzer.h"

#include <QDebug>


CYKSyntacticalAnalyzer::CYKSyntacticalAnalyzer()
{
}

Sentences CYKSyntacticalAnalyzer::analyze(const AmbigiousStringVector &fv, const CNFGrammar &grammar)
{
    Sentences result;
    if (fv.isEmpty()) {
        qWarning("CYK Syntactical Analyzer trying to analyze empty sentence");
        return result;
    }

    const int n = fv.size();    // n == number of rows in CYK-Algorithm

    CYKMatrixVector *p_lastSubmatrixVector = NULL;
    for (int j = n - 1; j >= 0; --j) {
        CYKMatrixVector *p_newSubmatrixVector = getSubmatrixVector(fv, j, p_lastSubmatrixVector, grammar);

        if (p_lastSubmatrixVector)
            delete p_lastSubmatrixVector;
        p_lastSubmatrixVector = p_newSubmatrixVector;
    }
    if (p_lastSubmatrixVector == NULL)
        return result;

    const quint32 sz = p_lastSubmatrixVector->size();
    for (quint32 i = 0; i < sz; ++i)
    {
        qDebug() << QString("root[%1]").arg(QString::number(i)) << p_lastSubmatrixVector->at(i)->at(0,0);
        if (p_lastSubmatrixVector->at(i)->at(0,0).contains(_ROOT))
        {
            result.append(
                        grammar.toSentence(
                            p_lastSubmatrixVector->at(i)->row(n-1)));
        }
    }
    return result;
}

//CYKMatrix CYKSyntacticalAnalyzer::allocateMatrix(int n)
//{
//    CYKMatrix matrix = NULL;

//    if (n > 0)
//        matrix = new NonterminalRow[n];

//    for (int i = 0; i < n; ++i)
//        matrix[i] = new NonterminalList[i + 1];

//    return matrix;
//}

//void CYKSyntacticalAnalyzer::deallocateMatrix(const CYKMatrix &matrix, int size)
//{
//    if (matrix == NULL) {
//        qWarning("deallocateMatrix matrix is NULL");
//        return;
//    }
//    for (int i = 0; i < size; ++i)
//        delete [] matrix[i];
//    delete [] matrix;
//}

CYKMatrixVector *CYKSyntacticalAnalyzer::getSubmatrixVector(const AmbigiousStringVector &fv,
                                                            int from,
                                                            const CYKMatrixVector *lastEvaluated,
                                                            const CNFGrammar &grammar)
{
    CYKMatrixVector *res = NULL;
    if (fv.isEmpty())
        return res;

    const int n = fv.size();
    const int size = n - from;
    qDebug() << "evaluating" << size;

    if (lastEvaluated) {
        // BODY
        const int newTerminalCount = fv.at(from).size();
        Q_ASSERT(newTerminalCount > 0);
        const int oldMatrixesCount = lastEvaluated->size();
        const int newMatrixesCount = newTerminalCount * oldMatrixesCount;
//        qDebug() << "CYK newMatrixesCount" << newMatrixesCount;

        res = new CYKMatrixVector(newMatrixesCount);
        Q_ASSERT(!res->isEmpty());

        for (int i = 0; i < newTerminalCount; ++i) {
            // For each terminal own matrixes
            const Terminal &term = fv.at(from).at(i);
            for (int j = 0; j < oldMatrixesCount; ++j) {
                // For each old matrixes and new terminal - new matrix
                CYKMatrixPtr newMatrix = lastEvaluated->at(i*oldMatrixesCount + j)->produceNext();

                const int lastRowIndex = newMatrix->size() - 1;
                // produce first rule
                NonterminalList tmp = grammar.getNonterminals(term);
                newMatrix->columnAt(lastRowIndex) = tmp;
                qDebug() << term << newMatrix->columnAt(lastRowIndex).size();
//                qDebug() << QString("added to m[%1](%2,0)")
//                            .arg(QString::number(newMatrix->size()))
//                            .arg(QString::number(lastRowIndex));
//                qDebug() << "FILLED NONTERMINALS";

                // produce other rules
                for (int rowIndex = lastRowIndex - 1; rowIndex >= 0; --rowIndex) {
                    for (int h = 1; rowIndex + h <= lastRowIndex; ++h) {
                        // TODO

                        NonterminalList &tmp1 = newMatrix->at(
                                    lastRowIndex + 1 - h,
                                    lastRowIndex - rowIndex + 1 - h);

                        qDebug() << QString("between (%1,0) (%2, %3)")
                                    .arg(QString::number(rowIndex+h))
                                    .arg(QString::number(lastRowIndex + 1 - h))
                                    .arg(QString::number(lastRowIndex - rowIndex + 1 - h));
                        NonterminalList tmp = grammar.getNonterminals(
                                    newMatrix->at(rowIndex+h, 0),
                                    tmp1);
                        qDebug() << "produced size:"<<tmp.size();

                        newMatrix->columnAt(rowIndex).append( tmp );
//                        qDebug() << QString("added to m[%1](%2,0) (sz:%3)")
//                                    .arg(newMatrix->size())
//                                    .arg(rowIndex)
//                                    .arg(newMatrix->columnAt(rowIndex).size());
                    }
                }
                (*res)[i*oldMatrixesCount + j] = newMatrix;

            }
        }
//        delete [] lastEvaluated;
    }
    else {
        // BASE
        Q_ASSERT(size == 1);

        res = new CYKMatrixVector(fv.at(from).size());
        Q_ASSERT(!res->isEmpty());

        for (int i = 0; i < res->size(); ++i) {
            CYKMatrixPtr newMatrix = new CYKMatrix();

            (*res)[i] = newMatrix;
            newMatrix->column()->append(grammar.getNonterminals(fv.at(from)[i]));
            qDebug() << fv.at(from)[i] << newMatrix->column()->size();
        }
    }

    return res;
}
