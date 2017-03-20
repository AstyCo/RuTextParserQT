#include "cyksyntacticalanalyzer.h"

#include <QDebug>


CYKSyntacticalAnalyzer::CYKSyntacticalAnalyzer(const QString &filename = QString())
{
    if (!filename.isEmpty()) {
        _dumpFilename = filename;
    }
    else {
        static QString dumpFilename("dumps/cyk_synt_analyzer.dump");

#ifdef RUTEXTPARSERQT_DIR
        _dumpFilename = RUTEXTPARSERQT_DIR + dumpFilename;
#else
        qWarning("RUTEXTPARSERQT_DIR not defined");

        QString manualPath = "../../RuTextParserQT/";
        _dumpFilename = manualPath + dumpFilename;
#endif
    }

    loadFromDump(filename);
}

CYKSyntacticalAnalyzer::CYKSyntacticalAnalyzer(const FeatureMapper &fm, const LinkMapper &lm)
{

}

QList<RuleNode> CYKSyntacticalAnalyzer::analyze(const AmbigiousStringVector &fv, const CNFGrammar &grammar)
{
    QList<RuleNode> result;
    if (fv.isEmpty()) {
        qWarning("CYK Syntactical Analyzer trying to analyze empty sentence");
        return result;
    }

    CYKMatrix matrix(fv.size());
    int last = fv.size() - 1;

    initLastRow(fv, matrix);
}

QList<RuleNode> CYKSyntacticalAnalyzer::analyze(const QStringList &v, const CNFGrammar &grammar)
{
    QList<RuleNode> result;
    if (v.isEmpty()) {
        qWarning("CYK Syntactical Analyzer trying to analyze empty sentence");
        return result;
    }

    CYKMatrix matrix(v.size());
    int last = v.size() - 1;

}

void CYKSyntacticalAnalyzer::loadFromDump(const QString &filename)
{

}

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

void CYKSyntacticalAnalyzer::fillLastRow(const AmbigiousStringVector &fv, CYKMatrix &matrix)
{
    int last = fv.size() - 1;
    for (int i=0; i < fv.size(); ++i) {
        foreach (const QString &fw, fv)
            matrix.at(last, i).append(CYKCellRecord());
    }
}
