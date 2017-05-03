#ifndef FIRSTCFGPARSER_H
#define FIRSTCFGPARSER_H

#include "syntagrus_parser_global.h"
#include "firstcfg.h"
#include "rutextparser_extensions/dumpable.h"
#include "internal/base-types.h"
#include "corpora_parsing/treecorpora.h"

#include <QMap>
#include <QList>

class SYNTAGRUS_PARSERSHARED_EXPORT FirstCFGParser
{
    typedef QPair<RuleDirection, QPair<linkID, featureID> > TDep;

    FirstCFG _cfg;

    QMap<nonterminal, QSet<QList<linkID> > > _linksForRules;
public:
    FirstCFGParser(QString filename)
        : _cfg(filename)
    {

    }

    FirstCFG *getCFG() { return &_cfg;}

    void parseTree(const TreeCorpora &treeCorpora)
    {
        _cfg.clear();
        foreach(const SentenceInCorpora &sentence, treeCorpora.sentencesBySize())
            parseNode(sentence.root(),
                      treeCorpora.featureMapper(),
                      treeCorpora.linkMapper());
    }
    void parseNode(const RecordNode *node,
                   const FeatureMapper &fmapper,
                   const LinkMapper &lmapper)
    {
        if (!node) {
            printWarning("FirstCFGParser::parseNode: null node");
            return;
        }

        featureID srcFid = fmapper.index(node->record()._feat);

        if (!node->childNodes().isEmpty()) {
            QList<TDep> body;

            for (int i=0; i < node->childNodes().size(); ++i) {
                featureID depFEAT = fmapper.index(node->childNodes().at(i)->record()._feat);
                linkID depLINK = lmapper.index(node->childNodes().at(i)->record()._link);
                TDep newDep = qMakePair(
                            produceDirection(node->record().before(node->childNodes().at(i)->record())),
                            qMakePair(depLINK, depFEAT));

                body.append(newDep);

                // parse recursively
                parseNode(node->childNodes().at(i), fmapper, lmapper);
            }
            if (node->record()._dom == -1)  // if root
                _cfg.addRoot(body);

            _cfg.add(srcFid, body);
        }
    }
};

#endif // FIRSTCFGPARSER_H
