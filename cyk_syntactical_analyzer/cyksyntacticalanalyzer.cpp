#include "cyksyntacticalanalyzer.h"


CYKSyntacticalAnalyzer::CYKSyntacticalAnalyzer()
{
}

Sentences CYKSyntacticalAnalyzer::analyze(const AmbigiousStringVector &fv, const CNFGrammar &grammar)
{
    int n = fv.size();
    AmbigiousStringVector rowCYK;
    Sentences result;
    if (n == 0)
        return result;

    const QStringList &feats = fv.first();
    foreach (const QString &feat, fv) {
        QStringList nonterminals = grammar.getNonterminals(feat);

    }

    for (int i = n - 1; i > 0; --i) {

    }

    while (n > 0) {


    }
}
