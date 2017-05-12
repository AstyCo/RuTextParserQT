#ifndef SECONDCFGPARSER_H
#define SECONDCFGPARSER_H

#include "syntagrus_parser_global.h"
#include "firstcfg.h"
#include "secondcfg.h"
#include "rutextparser_extensions/dumpable.h"
#include "internal/base-types.h"
#include "corpora_parsing/treecorpora.h"


#include <QMap>
#include <QList>
#include <QSet>

class SYNTAGRUS_PARSERSHARED_EXPORT SecondCFGParser
{
public:
    typedef QPair<linkID, featureID> TTerm;
    typedef QPair<RuleDirection, TTerm> TDep;

    FirstCFG *_firstCFG;
    QVector<QVector<bool> > _termMatrix;
    QSet<QPair<TTerm, TTerm> > _depends;
    SecondCFG _cfg;
public:
    SecondCFGParser(QString filename)
        : _firstCFG(NULL), _cfg(filename) {
    }

    SecondCFG *getCFG() { return &_cfg;}
    void setFirstCFG(FirstCFG *cfg) { _firstCFG = cfg;}

    void parseTree(const TreeCorpora &treeCorpora)
    {
        if (_firstCFG == NULL) {
            qWarning("Parsing failed, _firstCFG == NULL");
            return;
        }
//        _cfg.clear();

        const FeatureMapper &fmapper = treeCorpora.featureMapper();
        const LinkMapper &lmapper = treeCorpora.linkMapper();

        _depends.clear();
        initTermMatrix(fmapper, lmapper);

        foreach (const SentenceInCorpora &sentence, treeCorpora.sentencesBySize())
            preParseNode(sentence.root(), fmapper, lmapper);
//        foreach (const SentenceInCorpora &sentence, treeCorpora.sentencesBySize())
//            parseNode(sentence.root(), fmapper, lmapper);
        qDebug() << QString("termMatrix true count %1").arg(calcTermCount());

        buildCFG(fmapper, lmapper);
    }

    void preParseNode(const RecordNode *node,
                      const FeatureMapper &fmapper,
                      const LinkMapper &lmapper)
    {
        if (!node) {
            printWarning("FirstCFGParser::parseNode: null node");
            return;
        }

        if (!node->childNodes().isEmpty()) {
            linkID srcLINK;
            featureID srcFEAT;
            TTerm srcTerm;

            bool root = node->record()._dom == -1;
            if (!root) {
                srcLINK = lmapper.index(node->record()._link);
                srcFEAT = fmapper.index(node->record()._feat);
                srcTerm = qMakePair<linkID, featureID>(srcLINK, srcFEAT);
            }

            for (int i=0; i < node->childNodes().size(); ++i) {
                linkID depLINK = lmapper.index(node->childNodes().at(i)->record()._link);
                featureID depFEAT = fmapper.index(node->childNodes().at(i)->record()._feat);
                TTerm depTerm = qMakePair<linkID, featureID>(depLINK, depFEAT);

                _termMatrix[depLINK][depFEAT] = true;
                if (!root)
                    // insert in depends
                    _depends.insert(qMakePair(srcTerm, depTerm));

                // parse recursively
                preParseNode(node->childNodes().at(i), fmapper, lmapper);
            }
        }
    }

    void buildCFG(const FeatureMapper &/*fmapper*/,
                  const LinkMapper &/*lmapper*/)
    {
        long long contains = 0, notcontains = 0;
        nonterminal rootNTFirst = _firstCFG->rootNT();
        const UniqueVector<nonterminal, featureID> &terminalRules = _firstCFG->terminalRules();
        const QMap<nonterminal, QList<QList<QPair<RuleDirection, nonterminal> > > > &nonterminalRules = _firstCFG->nonterminalRules();
        const QMap<nonterminal, QList<QSet<QList<linkID> > > > &linksForRules = _firstCFG->linksForRules();
        QMap<nonterminal, QList<QList<QPair<RuleDirection, nonterminal> > > >::const_iterator
                it(nonterminalRules.constBegin());

        QMap<linkID, QList<QList<TDep> > > bodiesForSynto;

        while (it != nonterminalRules.constEnd()) {
            if (it.key() == rootNTFirst) {
                // TODO
                for (int i=0; i < it.value().size(); ++i) {
                    const QList<QPair<RuleDirection, nonterminal> > &body = it.value()[i];
                    QList<QList<linkID> > bodyLinks = linksForRules[it.key()][i].values();
                    for (int j=0; j < bodyLinks.size(); ++j) {
                        QList<TDep> newBody;
                        for (int k=0; k < body.size(); ++k) {
                            qDebug() << "1" << body.size();
                            qDebug() << "2" << bodyLinks.at(j).size() << endl;
                            linkID depLINK = bodyLinks.at(j).at(k);
                            featureID depFEAT = terminalRules.value(body.at(k).second);

                            newBody.append(qMakePair(body.at(k).first,
                                                     qMakePair(depLINK, depFEAT)));
                        }
                        _cfg.addRoot(newBody);
                    }

                }
            }
            else {
                featureID fid = terminalRules.value(it.key());

                for (linkID synto=0; synto < _termMatrix.size(); ++synto) {
                    if (_termMatrix[synto][fid] == false)
                        continue;
                    TTerm srcTerm = qMakePair(synto, fid);

                    for (int i=0; i < it.value().size(); ++i) {
                        const QList<QPair<RuleDirection, nonterminal> > &body = it.value()[i];
                        QList<QList<linkID> > bodyLinks = linksForRules[it.key()][i].values();
                        for (int j=0; j < bodyLinks.size(); ++j) {
                            QList<TDep> newBody;
                            bool failed = false;
                            for (int k=0; k < body.size(); ++k) {
                                linkID depLINK = bodyLinks.at(j).at(k);
                                featureID depFEAT = terminalRules.value(body.at(k).second);
                                TTerm depTerm = qMakePair(depLINK, depFEAT);

                                if (!_depends.contains(qMakePair(srcTerm, depTerm))) {
                                    failed = true;
                                    break;
                                }

                                newBody.append(qMakePair(body.at(k).first, depTerm));
                            }
                            if (!failed) {
                                _cfg.add(srcTerm, newBody);
                                ++contains;
                            }
                            else {
                                ++notcontains;
                            }
                        }

                    }
                }
            }
            ++it;
        }
        qDebug().noquote() << "building SecondCFG finished" << endl
                 << "\tcontains" << contains
                 << "\tnot contains" << notcontains;
    }

    void parseNode(const RecordNode *node,
                   const FeatureMapper &fmapper,
                   const LinkMapper &lmapper)
    {
        if (!node) {
            printWarning("FirstCFGParser::parseNode: null node");
            return;
        }

        linkID srcLINK = lmapper.index(node->record()._link);
        featureID srcFEAT = fmapper.index(node->record()._feat);
        TTerm srcTerm = qMakePair<linkID, featureID>(srcLINK, srcFEAT);

        if (!node->childNodes().isEmpty()) {
            QList<TDep> body;

            for (int i=0; i < node->childNodes().size(); ++i) {
                linkID depLINK = lmapper.index(node->childNodes().at(i)->record()._link);
                featureID depFEAT = fmapper.index(node->childNodes().at(i)->record()._feat);
                TTerm depTerm = qMakePair<linkID, featureID>(depLINK, depFEAT);
                TDep newDep = qMakePair(
                            produceDirection(node->record().before(node->childNodes().at(i)->record())),
                            depTerm);

                body.append(newDep);

                // insert in depends
                _depends.insert(qMakePair(srcTerm, depTerm));
                // parse recursively
                parseNode(node->childNodes().at(i), fmapper, lmapper);
            }
            if (node->record()._dom == -1) { // if root
                _cfg.addRoot(body);
            }
            else {  // for second grammar root depends produce only root rules
                _cfg.add(srcTerm, body);
            }
        }
    }

private:
    void initTermMatrix(const FeatureMapper &fmapper, const LinkMapper &lmapper)
    {
        _termMatrix.clear();
        _termMatrix.resize(lmapper.size());
        for (int i=0; i < _termMatrix.size(); ++i) {
            _termMatrix[i].resize(fmapper.size());
            for (int j=0; j < _termMatrix[i].size(); ++j)
                _termMatrix[i][j] = false;
        }
    }

    int calcTermCount() const
    {
        int result = 0;
        for (int i=0; i < _termMatrix.size(); ++i) {
            for (int j=0; j < _termMatrix[i].size(); ++j)
                if (_termMatrix[i][j])
                    ++result;
        }
        return result;
    }



};

#endif // SECONDCFGPARSER_H
