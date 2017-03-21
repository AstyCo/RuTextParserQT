#include "cyksyntacticalanalyzer.h"

#include <QDebug>


CYKSyntacticalAnalyzer::CYKSyntacticalAnalyzer(const QString &fmapper, const QString &lmapper)
{
    if (!fmapper.isEmpty())
        _fmapper.setFilename(fmapper);

    _fmapper.load();

    if (!lmapper.isEmpty())
        _lmapper.setFilename(lmapper);

    _lmapper.load();
}

CYKSyntacticalAnalyzer::CYKSyntacticalAnalyzer(const FeatureMapper &fm, const LinkMapper &lm)
    : _fmapper(fm), _lmapper(lm)
{

}

QList<ListRuleNode> CYKSyntacticalAnalyzer::analyze(const AmbigiousStringVector &fv, const CNFGrammar &grammar)
{
    QList<ListRuleNode> result;
    if (fv.isEmpty()) {
        qWarning("CYK Syntactical Analyzer trying to analyze empty sentence");
        return result;
    }

    CYKMatrix matrix(fv.size());
    int last = fv.size() - 1;

    fillLastRow(fv, matrix);

}

QList<ListRuleNode> CYKSyntacticalAnalyzer::analyze(const QStringList &v, const CNFGrammar &grammar)
{
    QList<ListRuleNode> result;
    if (v.isEmpty()) {
        qWarning("CYK Syntactical Analyzer trying to analyze empty sentence");
        return result;
    }

    CYKMatrix matrix(v.size());
    int last = v.size() - 1;

}

void CYKSyntacticalAnalyzer::fillLastRow(const AmbigiousStringVector &fv, CYKMatrix &matrix)
{
    int last = fv.size() - 1;
    for (int i=0; i < fv.size(); ++i) {
        foreach (const QString &fw, fv[i])
            matrix.at(last, i).append(CYKCellRecord());
    }
}

//                for (int rowIndex = lastRowIndex - 1; rowIndex >= 0; --rowIndex) {
//                    for (int h = 1; rowIndex + h <= lastRowIndex; ++h) {
//                        NonterminalList &tmp1 = newMatrix->at(
//                                    lastRowIndex + 1 - h,
//                                    lastRowIndex - rowIndex + 1 - h);

void CYKSyntacticalAnalyzer::calcCell(CYKMatrix &matrix, const int &i, const int &j, const CNFGrammar &grammar)
{
    short sz = matrix.size();
    short h  = sz - i - 1;

    for (int p = 1; p <= h; ++p) {
        CYKCell &leftCell = matrix.at(i+p, j);
        CYKCell &rightCell = matrix.at(i+h-p, j+h+1-p);


    }

}
