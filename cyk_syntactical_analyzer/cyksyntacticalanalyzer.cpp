#include "cyksyntacticalanalyzer.h"
#include "rutextparser_extensions/rutextparser_extensions.h"

#include <QDebug>

//void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
//{
//    QByteArray localMsg = msg.toLocal8Bit();
//    switch (type) {
//    case QtDebugMsg:
//        fprintf(stderr, "%s", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtInfoMsg:
//        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtWarningMsg:
//        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtCriticalMsg:
//        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtFatalMsg:
//        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        abort();
//    }
//}

CYKSyntacticalAnalyzer::CYKSyntacticalAnalyzer(const QString &fmapper, const QString &lmapper)
{
//    qInstallMessageHandler(myMessageHandler);
    if (!ExtensionsLogs::Logs::registerLogStream("CYK_logs.n++"))
        Q_ASSERT(false);

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
    if (!ExtensionsLogs::Logs::registerLogStream("CYK_logs.n++"))
        Q_ASSERT(false);
}

QList<QSharedPointer<RuleNode> > CYKSyntacticalAnalyzer::analyze(const AmbigiousStringVector &fv, const CNFGrammar &grammar)
{
    QList<QSharedPointer<RuleNode> > result;
    if (fv.isEmpty()) {
        qWarning("CYK Syntactical Analyzer trying to analyze empty sentence");
        return result;
    }

//    *ExtensionsLogs::Logs::log("CYK_logs.n++") << "analyzing fv\n";
    CYKMatrix matrix(fv.size());
    int last = fv.size() - 1;

    fillLastRow(fv, matrix);
//    *ExtensionsLogs::Logs::log("CYK_logs.n++") << "last row filled\n";
    for (int j = last-1; j >= 0; --j) {
        for (int i = last-1; i >= j; --i) {
            calcCell(matrix, i, j, grammar);
//            *ExtensionsLogs::Logs::log("CYK_logs.n++") << QString("cell(%1,%2) size:%3")
//                        .arg(QString::number(i))
//                        .arg(QString::number(j))
//                        .arg(QString::number(matrix.at(i,j).size())) << endl;
        }
    }
//    *ExtensionsLogs::Logs::log("CYK_logs.n++") << "cells calculated" << endl;
    CYKCell &rootCell = matrix.at(0,0);
    CYKCellConstIterator i(rootCell.constBegin());
    QVector<Scored> rootScores = grammar.rootScore();
    while (i!=rootCell.constEnd()) {
        static int kk = 0;
        static int notaddedkk = 0;
        static int addedkk = 0;
        QSharedPointer<RuleNode> rn = i.value();
        const SimpleRuleNode *node = grammarGetNodeWidth(i.key(), *rn, grammar);
//        qDebug() << rn->toString(grammar.rulesByID(), _fmapper, _lmapper);
        if ( node && node->score > 0.5) {
            qreal rootScore = node->rootScore.score;
            if (rootScore < 0.5) {
//                *ExtensionsLogs::Logs::log("CYK_logs.n++") << "not a root" << endl;
//                *ExtensionsLogs::Logs::log("CYK_logs.n++") << "not added " <<  notaddedkk++ << ' ' << kk++ << endl;
//                *ExtensionsLogs::Logs::log("CYK_logs.n++") << rn->toString(grammar.rulesByID(), _fmapper, _lmapper);
                ++i;
                continue;
            }
            else {
                result.append(i.value());
//                *ExtensionsLogs::Logs::log("CYK_logs.n++") << _fmapper.value(i.key()) << " added with score " << rootScore << endl;
//                *ExtensionsLogs::Logs::log("CYK_logs.n++")  << "added " <<addedkk++ << ' ' << kk++ << endl;
            }
//            *ExtensionsLogs::Logs::log("CYK_logs.n++") << rn->toString(grammar.rulesByID(), _fmapper, _lmapper);
        }
//        *ExtensionsLogs::Logs::log("CYK_logs.n++") << "not added " <<  notaddedkk++ << ' ' << kk++ << endl;
//        *ExtensionsLogs::Logs::log("CYK_logs.n++") << rn->toString(grammar.rulesByID(), _fmapper, _lmapper) << endl;
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

//ListRuleID CYKSyntacticalAnalyzer::produceCYKSequence(const RuleNode &rn, const CNFGrammar &grammar)
//{
//    ListRuleID result;
//    QList<RuleLink> &childs = rn.rules();
//    if (childs.isEmpty()) {
//        Q_ASSERT(false);
//        return result;
//    }
//    const QVector<ScoredChomskyRuleRecord> &rulesByID=grammar.rulesByID();
//    // find first right rule
//    int i = 0;
//    for (;i<childs.size(); ++i) {
//        if (rulesByID[childs.at(i).id].rule._isRightRule)
//            break;
//    }
//    if (i < childs.size()) {
//        // add right rules
//        for (int j=i;j<childs.size(); ++j)
//            result.append(childs.at(j).id);
//    }
//    if (i > 0) {
//        // add left rules (reverted)
//        for (int j=i-1; j >= 0; --j)
//            result.append(childs.at(j).id);
//    }
//    return result;
//}

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
//        *ExtensionsLogs::Logs::log("CYK_logs.n++") << QString("cell(%1,%2) size:%3")
//                    .arg(QString::number(last))
//                    .arg(QString::number(i))
//                    .arg(QString::number(matrix.at(last,i).size())) << endl;
    }
}

void CYKSyntacticalAnalyzer::calcCell(CYKMatrix &matrix, const int &i, const int &j, const CNFGrammar &grammar)
{
//    *ExtensionsLogs::Logs::log("CYK_logs.n++") << QString("calculating cell (%1,%2)\n").arg(QString::number(i)).arg(QString::number(j));
    short sz = matrix.size();
    short h  = sz - i - 1;

    CYKCell &cell(matrix.at(i, j));
    QSet<ruleID> resultRules;

    _debugCellRefusedDepth = _debugCellRefusedWidth = _debugCellTotal = _debugCellRefusedLogical = 0;

    for (int p = 0; p < h; ++p) {
        const CYKCell &leftCell = matrix.at(i+p+1, j);
        const CYKCell &rightCell = matrix.at(i+h-p, j+h-p);

        CYKCellConstIterator l(leftCell.constBegin());
        while (l != leftCell.constEnd()) {
            const QVector<ListRuleID> &rulesWithLeft(grammar.rulesByRightIDsHash()[l.key()]);
            CYKCellConstIterator r(rightCell.constBegin());
            while (r != rightCell.constEnd()) {
//                *ExtensionsLogs::Logs::log("CYK_logs.n++") << QString("[%5,%6](%1,%2)(%3,%4)")
//                            .arg(QString::number(i+p+1))
//                            .arg(QString::number(j))
//                            .arg(QString::number(i+h-p))
//                            .arg(QString::number(j+h-p))
//                            .arg(i).arg(j)
//                          <<  " looking for rule between "<<_fmapper.value(l.key())
//                         <<" and " <<_fmapper.value(r.key())
//                        << QString(" [%1]").arg(QString::number(rulesWithLeft[r.key()].size())) << endl;
//                if (rulesWithLeft[r.key()].size() > 0) {
//                    foreach (const ruleID &id, rulesWithLeft[r.key()]) {
//                        *ExtensionsLogs::Logs::log("CYK_logs.n++") << '\t' <<grammar.rulesByID()[id].toString(_fmapper, _lmapper) << endl;
//                    }
//                }
                addRecord(cell, l.value(), r.value(),
                          rulesWithLeft[r.key()], grammar);
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
                                       const CNFGrammar &grammar)
{
    qreal totalScore = 0;
    for (int i=0; i < scoredRuleIDs.size(); ++i) {
        const ScoredChomskyRuleRecord &ruleRecord = grammar.rulesByID()[scoredRuleIDs.at(i)];
        totalScore += ruleRecord.score;
    }

    for (int i=0; i < scoredRuleIDs.size(); ++i) {
        const ScoredChomskyRuleRecord &ruleRecord = grammar.rulesByID()[scoredRuleIDs.at(i)];
        const QSharedPointer<RuleNode> &src = (ruleRecord.rule._isRightRule ? l : r);
//        QSharedPointer<RuleNode> src(new RuleNode(*srcL));
        const QSharedPointer<RuleNode> &dep = (ruleRecord.rule._isRightRule ? r : l);
        featureID fid = ruleRecord.rule._sourceFID;
        QSharedPointer<RuleNode> rn;
        if (src->isLeaf()) {
            rn = QSharedPointer<RuleNode>(new RuleNode(scoredRuleIDs.at(i), dep, 1));
//            cell.insert(fid, rn);
        }
        else {
            rn = QSharedPointer<RuleNode>(new RuleNode(*src));
            rn->append(RuleLink(scoredRuleIDs.at(i), dep));
        }
        const SimpleRuleNode *node = grammarGetNodeWidth(fid, *rn, grammar);
        if (node == NULL) {
//                qDebug() << "NOT CONTAINS";
//            *ExtensionsLogs::Logs::log("CYK_logs.n++") << QString("[T:%1,D:%2,W:%3,L:%4] REFUSED WIDTH ")
//                                                          .arg(++_debugCellTotal)
//                                                          .arg(_debugCellRefusedDepth)
//                                                          .arg(++_debugCellRefusedWidth)
//                                                          .arg(_debugCellRefusedLogical)
//                                                       << rn->toString(grammar.rulesByID(), _fmapper, _lmapper) << endl;
            rn->delta()++;
        }
        else {
            if (!dep->rules().isEmpty()) {
                const SimpleRuleNode *depNode = grammarGetNodeWidth(ruleRecord.rule._dependFID, *dep, grammar);
                if (depNode == NULL) {
                    Q_ASSERT(false);
                }
                else {
                    if (depNode->score < 0.5) {
//                        *ExtensionsLogs::Logs::log("CYK_logs.n++")  << QString("[T:%1,D:%2,W:%3,L:%4] REFUSED LOGICAL ")
//                                                                       .arg(++_debugCellTotal)
//                                                                       .arg(_debugCellRefusedDepth)
//                                                                       .arg(_debugCellRefusedWidth)
//                                                                       .arg(++_debugCellRefusedLogical)
//                                                                    << rn->toString(grammar.rulesByID(), _fmapper, _lmapper) << endl;
                        rn->delta()++;
                    }
                }
            }
            if (!grammarContainsRuleDepth(scoredRuleIDs.at(i), dep.data(), grammar)) {
//                *ExtensionsLogs::Logs::log("CYK_logs.n++")  << QString("[T:%1,D:%2,W:%3,L:%4] REFUSED DEPTH ")
//                                                               .arg(++_debugCellTotal)
//                                                               .arg(++_debugCellRefusedDepth)
//                                                               .arg(_debugCellRefusedWidth)
//                                                               .arg(_debugCellRefusedLogical)
//                                                            << rn->toString(grammar.rulesByID(), _fmapper, _lmapper) << endl;
                rn->delta()++;
            }
        }

        if (rn->delta() > 0) {
//            static long kk = 0;
//            qDebug() << "NOT CORRECT RULE" << ++kk;
            continue;
        }
        else {
//            *ExtensionsLogs::Logs::log("CYK_logs.n++") << QString("[T:%1,D:%2,W:%3,L:%4] ALLOWED ")
//                                                          .arg(++_debugCellTotal)
//                                                          .arg(_debugCellRefusedDepth)
//                                                          .arg(_debugCellRefusedWidth)
//                                                          .arg(_debugCellRefusedLogical)
//                                                       << rn->toString(grammar.rulesByID(), _fmapper, _lmapper) << endl;
//            static long kk = 0;
//            qDebug() << "correct rule" << ++kk;
        }
        if (!cell.contains(fid, rn)) {
            cell.insert(fid, rn);
        }
        else {
            CYKCellIterator i(cell.find(fid, rn));
            i.value()->increaseScore();
            qDebug() << "cell contains" << fid;
        }

    }
}

bool CYKSyntacticalAnalyzer::grammarContainsRuleDepth(ruleID rid, const RuleNode *dep, const CNFGrammar &grammar) const
{
    foreach (const RuleLink &link, dep->rules()) {
        QList<ruleID> ids;
        ids << link.id;
        ids << rid;
        if (!grammar.conseqRules().contains(ids))
            return false;
    }
    return true;
}

const SimpleRuleNode *CYKSyntacticalAnalyzer::grammarGetNodeWidth(const featureID &fid, const RuleNode &rn, const CNFGrammar &grammar) const
{
    // CYK is linear, so we should check only for rn.rules from begining, or from ending (no need to check from middle)
    // firstly, check for STRAIGHT
    ListRuleID ordered;
    for (int i=0; i<rn.rules().size(); ++i)
        ordered.append(rn.rules().at(i).id);

    return grammar.rulesByFeatureID()[fid].node(ordered);
}

const SimpleRuleNode *CYKSyntacticalAnalyzer::grammarFinalScore(const featureID &fid, const RuleNode &rn, const CNFGrammar &grammar) const
{
    ListRuleID sorted;
    for (int i=0; i<rn.rules().size(); ++i)
        sorted.append(rn.rules().at(i).id);

    const SimpleRuleNode *node = grammar.rulesByFeatureID()[fid].node(sorted);
    if (node && node->rootScore.score > 0.5) {
        // this means rule is final
        return node;
//        return true;
    }
    return NULL;
}

//featureID CYKSyntacticalAnalyzer::srcID(const CYKCellRecord &r, const CNFGrammar &grammar)
//{
//    if (r._topRules.isEmpty())
//        return -1;
//    return grammar.rulesByID()[r._topRules.first().id].rule.sourceRule;
//}
