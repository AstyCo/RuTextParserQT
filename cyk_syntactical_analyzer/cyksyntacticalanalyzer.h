#ifndef CYKANALYZER_H
#define CYKANALYZER_H

#include "cyk_syntactical_analyzer_global.h"
#include "grammar/cnfgrammar.h"


#include <QList>

typedef QList<QStringList> AmbigiousStringVector;
typedef QList<QStringList> Sentences;

typedef QList<QString> FeatureVector;

class CYK_SYNTACTICAL_ANALYZERSHARED_EXPORT CYKSyntacticalAnalyzer
{
public:
    CYKSyntacticalAnalyzer();

    Sentences analyze(const AmbigiousStringVector &fv, const CNFGrammar &grammar);

};

#endif // CYKANALYZER_H
