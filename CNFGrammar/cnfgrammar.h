#ifndef CNFGRAMMAR_H
#define CNFGRAMMAR_H

#include "cnfgrammar_global.h"

#include "rulecnfgrammar.h"

#include <QList>

class CNFGRAMMARSHARED_EXPORT CNFGrammar
{

public:
    CNFGrammar();

    void addRule(const RuleCNFGrammar &rule);

private:
    QList<RuleCNFGrammar> _rules;
};

#endif // CNFGRAMMAR_H
