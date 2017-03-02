#include "cnfgrammar.h"


CNFGrammar::CNFGrammar()
{
}

void CNFGrammar::addRule(const RuleCNFGrammar &rule)
{
    _rules.append(rule);
}
