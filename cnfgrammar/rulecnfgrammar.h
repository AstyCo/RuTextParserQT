#ifndef RULECNFGRAMMAR_H
#define RULECNFGRAMMAR_H

#include <QString>

struct RuleCNFGrammar
{
public:
    RuleCNFGrammar(const QString &left, const QString &first, const QString &second = QString());

    QString _left;
    QString _first;
    QString _second;
};

#endif // RULECNFGRAMMAR_H
