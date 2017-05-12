#include "simplycnfbuilder.h"


SimplyCNFBuilder::SimplyCNFBuilder(FirstCFG *cfg)
    :_cfg(cfg)
{
    _cnf.setNextFreeNT(_cfg->terminalRules().size());
}

void SimplyCNFBuilder::build()
{
    if (!_cfg)
        return;

    const UniqueVector<nonterminal, featureID> &terminalRules = _cfg->terminalRules();
    const QMap<nonterminal, Rules> &nonterminalRules = _cfg->nonterminalRules();

    _cnf.setTerminalRules(terminalRules);

    QMap<nonterminal, Rules>::const_iterator it(nonterminalRules.constBegin());

    while (it != nonterminalRules.constEnd()) {

        if (it.key() == _cfg->rootNT())
            produceRootRules(it.value());
        else {
            produceRules(it.key(), it.value());
        }

        ++it;
    }
    qDebug() << "B2";
    qDebug().noquote() << _cnf.toReport();
//    qDebug().noquote() << _cnf.toReportRules();

    _cnf.recursiveMinimize();
}

void SimplyCNFBuilder::produceRules(nonterminal srcNT, const SimplyCNFBuilder::Rules &rules)
{
    nonterminal topNT = srcNT;
    foreach (const RuleBody &body, rules) {
        nonterminal depNT;
        for (int i=0; i < body.size(); ++i) {
            if (body.size() - i > 1)
                depNT = _cnf.nextNT();
            else
                depNT = srcNT; // if last

            if (body.at(i).first == LeftRule)
                _cnf.add(topNT, body.at(i).second, depNT);
            else
                _cnf.add(topNT, depNT, body.at(i).second);

            topNT = depNT;
        }
    }
}

void SimplyCNFBuilder::produceRootRules(const SimplyCNFBuilder::Rules &rules)
{
    nonterminal topNT = _cfg->rootNT();
    foreach (const RuleBody &body, rules) {
        nonterminal depNT;
        for (int i=0; i < body.size(); ++i) {
            if (body.size() - i > 1)
                depNT = _cnf.nextNT();
            else
                depNT = body.last().second; // if last

            if (body.at(i).first == LeftRule)
                _cnf.add(topNT, body.at(i).second, depNT);
            else
                _cnf.add(topNT, depNT, body.at(i).second);

            topNT = depNT;
        }
    }
}
