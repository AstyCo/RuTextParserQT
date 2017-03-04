#include "rulecnfgrammar.h"

RuleCNFGrammar::RuleCNFGrammar()
{

}

RuleCNFGrammar::RuleCNFGrammar(const QString &left, const QString &first, const QString &second)
    :_left(left), _first(first), _second(second)
{

}

// serialization
QDataStream &operator<<(QDataStream &ds, const RuleCNFGrammar &rule)
{
    ds << rule._left;
    ds << rule._first;
    ds << rule._second;
    return ds;
}

// deserialization
QDataStream &operator>>(QDataStream &ds, RuleCNFGrammar &rule)
{
    ds >> rule._left;
    ds >> rule._first;
    ds >> rule._second;
    return ds;
}
