#include "rulecnfgrammar.h"

#include <QDebug>

RuleCNFGrammar::RuleCNFGrammar()
    :_type(UndefinedRule)
{
}

RuleCNFGrammar::RuleCNFGrammar(const Nonterminal &leftNonterminal, const Terminal &terminal)
    : _leftNonterminal(leftNonterminal), _terminal(terminal)
{
    if (_terminal == EPSILON_TERMINAL) {
        _type = EpsilonRule;
        qDebug() << "Epsilon rule not expected";
    }
    else {
        _type = TerminalRule;
    }
}

RuleCNFGrammar::RuleCNFGrammar(const Nonterminal &left, const Nonterminal &first, const Nonterminal &second)
    : _leftNonterminal(left), _firstNonterminal(first), _secondNonterminal(second)
{
    _type = NonterminalRule;
}

// serialization
QDataStream &operator<<(QDataStream &ds, const RuleCNFGrammar &rule)
{
    ds << rule._type;
    ds << rule._leftNonterminal;
    ds << rule._firstNonterminal;
    ds << rule._secondNonterminal;
    ds << rule._terminal;

    return ds;
}

// deserialization
QDataStream &operator>>(QDataStream &ds, RuleCNFGrammar &rule)
{
    ds >> rule._type;
    ds >> rule._leftNonterminal;
    ds >> rule._firstNonterminal;
    ds >> rule._secondNonterminal;
    ds >> rule._terminal;

    return ds;
}

QDataStream &operator<<(QDataStream &ds, const RuleCNFGrammar::RuleType &ruleType)
{
    ds << static_cast<short>(ruleType);

    return ds;
}

QDataStream &operator>>(QDataStream &ds, RuleCNFGrammar::RuleType &ruleType)
{
    short s;
    ds >> s;
    if (s > RuleCNFGrammar::RuleTypeSize) {
        qWarning("QDataStream failed to deserialize RuleType value");
        return ds;
    }

    ruleType = static_cast<RuleCNFGrammar::RuleType>(s);
    return ds;
}
