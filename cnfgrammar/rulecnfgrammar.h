#ifndef RULECNFGRAMMAR_H
#define RULECNFGRAMMAR_H

#include <QString>

struct RuleCNFGrammar
{
public:
    RuleCNFGrammar();

    QString _left;
    QString _first;
    QString _second;
};

#endif // RULECNFGRAMMAR_H
