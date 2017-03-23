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

QList<QSharedPointer<RuleNode> > CYKSyntacticalAnalyzer::analyze(const AmbigiousStringVector &fv, const CNFGrammar &grammar)
{
    QList<QSharedPointer<RuleNode> > result;
    if (fv.isEmpty()) {
        qWarning("CYK Syntactical Analyzer trying to analyze empty sentence");
        return result;
    }

    qDebug() << "analyzing fv";
    CYKMatrix matrix(fv.size());
    int last = fv.size() - 1;

    fillLastRow(fv, matrix);
    qDebug() << "last row filled";
    for (int j = last-1; j >= 0; --j) {
        for (int i = last-1; i >= j; --i) {
            calcCell(matrix, i, j, grammar);
            qDebug() << QString("cell(%1,%2) size:%3")
                        .arg(QString::number(i))
                        .arg(QString::number(j))
                        .arg(QString::number(matrix.at(i,j).size()));
        }
    }
    qDebug() << "cells calculated";
    CYKCell &rootCell = matrix.at(0,0);
    CYKCellConstIterator i(rootCell.constBegin());
    QVector<Scored> rootScores = grammar.rootScore();
    while (i!=rootCell.constEnd()) {
        qreal score = rootScores[i.key()].score;
        if (score == 0) {
            qDebug() << _fmapper.value(i.key()) << "not a root";
        }
        else {
            result.append(i.value());
            qDebug() << _fmapper.value(i.key()) << "added with score" << score;
        }

        ++i;
    }

    return result;
}

inline AmbigiousStringVector toAmbigious(const QStringList &sentence)
{
    AmbigiousStringVector res;
    for (int i=0; i < sentence.size(); ++i)
        res.append(QStringList(sentence.at(i)));
    return res;
}

QList<QSharedPointer<RuleNode> > CYKSyntacticalAnalyzer::analyze(const QStringList &v, const CNFGrammar &grammar)
{
    return analyze(toAmbigious(v), grammar);
}

void CYKSyntacticalAnalyzer::fillLastRow(const AmbigiousStringVector &fv, CYKMatrix &matrix)
{
    int last = fv.size() - 1;
    for (int i=0; i < fv.size(); ++i) {
        foreach (const QString &f, fv[i]) {
            featureID fid = _fmapper.index(f);
            if (_fmapper.isValid(fid))
                matrix.at(last, i).insert(fid, QSharedPointer<RuleNode>(new RuleNode(1))); // TODO score

            else
                Q_ASSERT(false);
        }
        qDebug() << QString("cell(%1,%2) size:%3")
                    .arg(QString::number(last))
                    .arg(QString::number(i))
                    .arg(QString::number(matrix.at(last,i).size()));
    }
}

void CYKSyntacticalAnalyzer::calcCell(CYKMatrix &matrix, const int &i, const int &j, const CNFGrammar &grammar)
{
    qDebug() << QString("calculating cell (%1,%2)").arg(QString::number(i)).arg(QString::number(j));
    short sz = matrix.size();
    short h  = sz - i - 1;

    CYKCell &cell(matrix.at(i, j));
    QSet<ruleID> resultRules;

    for (int p = 0; p < h; ++p) {
        const CYKCell &leftCell = matrix.at(i+p+1, j);
        const CYKCell &rightCell = matrix.at(i+h-p, j+h-p);

        CYKCellConstIterator l(leftCell.constBegin());
        while (l != leftCell.constEnd()) {
            const QVector<ListRuleID> &rulesWithLeft(grammar.rulesByRightIDsHash()[l.key()]);
            CYKCellConstIterator r(rightCell.constBegin());
            while (r != rightCell.constEnd()) {
//                qDebug() << QString("(%1,%2)(%3,%4)")
//                            .arg(QString::number(i+p+1))
//                            .arg(QString::number(j))
//                            .arg(QString::number(i+h-p))
//                            .arg(QString::number(j+h-p))
//                          <<  "looking for rule between"<<_fmapper.value(l.key())
//                         <<"and" <<_fmapper.value(r.key())
//                        << QString("[%1]").arg(QString::number(rulesWithLeft[r.key()].size()));
//                if (rulesWithLeft[r.key()].size() > 0) {
//                    foreach (const ruleID &id, rulesWithLeft[r.key()]) {
//                        qDebug() << grammar.rulesByID()[id].toString(_fmapper, _lmapper);
//                    }
//                }
                addRecord(cell, l.value(), r.value(),
                          rulesWithLeft[r.key()], grammar.rulesByID());
                r++;
            }
            l++;
        }

//        for (int l=0; l < leftCell.size(); ++l) {
//            QVector<ListScoredRuleID> &rulesWithLeft(grammar.rulesByRightIDsHash()[leftCell[l].fid]);
//            for (int r=0; r < rightCell.size(); ++r) {

//            }
//        }

    }

}

void CYKSyntacticalAnalyzer::addRecord(CYKCell &cell,
                                       const QSharedPointer<RuleNode> &l,
                                       const QSharedPointer<RuleNode> &r,
                                       const ListRuleID &scoredRuleIDs,
                                       const QVector<ScoredChomskyRuleRecord> &rulesByID)
{

    for (int i=0; i < scoredRuleIDs.size(); ++i) {
        const ScoredChomskyRuleRecord &ruleRecord = rulesByID[scoredRuleIDs.at(i)];
        const QSharedPointer<RuleNode> &src = (ruleRecord.rule._isRightRule ? l : r);
        const QSharedPointer<RuleNode> &dep = (ruleRecord.rule._isRightRule ? r : l);
        featureID fid = ruleRecord.rule._sourceFID;
        QSharedPointer<RuleNode> rn;
        if (src->isLeaf()) {
            rn = QSharedPointer<RuleNode>(new RuleNode(scoredRuleIDs.at(i), dep, 1));
//            cell.insert(fid, rn);
        }
        else {
            rn = QSharedPointer<RuleNode>(new RuleNode(1));
            rn->rules = src->rules;
            ExtensionsQtContainers::insert_sorted(rn->rules, RuleLink(scoredRuleIDs.at(i), dep));
//            rn->rules.append(RuleLink(scoredRuleIDs.at(i).id, dep));

//            if (!cell.contains(fid, rn)) {
//                cell.insert(fid, rn);
//            }
//            else {
//                qDebug() << "cell contains" << fid;
//            }
        }
        if (!cell.contains(fid, rn)) {
            cell.insert(fid, rn);
        }
        else {
            CYKCellIterator i(cell.find(fid, rn));
            i.value()->increaseScore();
//            qDebug() << "cell contains" << fid;
        }

    }
}

//featureID CYKSyntacticalAnalyzer::srcID(const CYKCellRecord &r, const CNFGrammar &grammar)
//{
//    if (r._topRules.isEmpty())
//        return -1;
//    return grammar.rulesByID()[r._topRules.first().id].rule.sourceRule;
//}
