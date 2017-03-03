#ifndef RULECNFGRAMMAR_H
#define RULECNFGRAMMAR_H

#include "cnfgrammar_global.h"

#include <QString>

struct CNFGRAMMARSHARED_EXPORT RuleCNFGrammar
{
public:
    RuleCNFGrammar();
    RuleCNFGrammar(const QString &left, const QString &first, const QString &second = QString());

    QString _left;
    QString _first;
    QString _second;

    friend QDataStream &operator<<(QDataStream &ds, const RuleCNFGrammar &rule);
    friend QDataStream &operator>>(QDataStream &ds, RuleCNFGrammar &gr);
};

QDataStream &operator<<(QDataStream &ds, const RuleCNFGrammar &rule);
QDataStream &operator>>(QDataStream &ds, RuleCNFGrammar &rule);

#endif // RULECNFGRAMMAR_H
