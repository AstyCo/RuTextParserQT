#ifndef RULECNFGRAMMAR_H
#define RULECNFGRAMMAR_H

#include "syntagrus_parser_global.h"

#include <QPair>
#include <QString>
#include <QStringList>
#include <QDataStream>

typedef quint32 Nonterminal;
typedef QList<Nonterminal> NonterminalList;
typedef QPair<Nonterminal, Nonterminal> NonterminalPair;

typedef QString Terminal;
typedef QStringList TerminalList;

#define EMPTY_NONTERMINAL           0
#define _ROOT                       1
#define FIRST_EMPTY_NONTERMINAL     10

#define EMPTY_TERMINAL              (QString())
#define EPSILON_TERMINAL            EMPTY_TERMINAL
#define FIRST_EMPTY_TERMINAL        (QString())


struct SYNTAGRUS_PARSERSHARED_EXPORT RuleCNFGrammar
{
public:

    enum RuleType
    {
        UndefinedRule   = 0,

        EpsilonRule     = 1,
        TerminalRule    = 2,
        NonterminalRule = 3,

        RuleTypeSize
    };


    RuleCNFGrammar();
    RuleCNFGrammar(const Nonterminal &leftNonterminal, const Terminal &terminal);
    RuleCNFGrammar(const Nonterminal &left, const Nonterminal &first, const Nonterminal &second = EMPTY_NONTERMINAL);

    bool isEpsilonRule() const;
    bool isTerminalRule() const;
    bool isNonterminalRule() const;

    RuleType _type;
    Nonterminal _leftNonterminal;
    Nonterminal _firstNonterminal;
    Nonterminal _secondNonterminal;
    Terminal _terminal;

    friend QDataStream &operator<<(QDataStream &ds, const RuleCNFGrammar &rule);
    friend QDataStream &operator>>(QDataStream &ds, RuleCNFGrammar &gr);
};

/// INLINE FUNCTIONS

inline bool RuleCNFGrammar::isEpsilonRule() const { return _type == EpsilonRule; }

inline bool RuleCNFGrammar::isTerminalRule() const { return _type == TerminalRule; }

inline bool RuleCNFGrammar::isNonterminalRule() const { return _type == NonterminalRule; }

/// Serialization / Deserialization

QDataStream &operator<<(QDataStream &ds, const RuleCNFGrammar &rule);
QDataStream &operator>>(QDataStream &ds, RuleCNFGrammar &rule);

QDataStream &operator<<(QDataStream &ds, const RuleCNFGrammar::RuleType &ruleType);
QDataStream &operator>>(QDataStream &ds, RuleCNFGrammar::RuleType &ruleType);


#endif // RULECNFGRAMMAR_H
