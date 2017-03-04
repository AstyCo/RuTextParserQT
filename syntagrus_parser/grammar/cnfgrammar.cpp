#include "cnfgrammar.h"

#include "rutextparser_extensions.h"

#include <QFile>
#include <QStringList>

CNFGrammar::CNFGrammar()
    : _dumpFilename("CNFGrammar.dump")
{
}

void CNFGrammar::append(const RuleCNFGrammar &rule)
{
    _rules.append(rule);

    fillCache(&_rules[_rules.size() - 1]);
}

void CNFGrammar::clear()
{
    _rules.clear();

    clearCache();
}

void CNFGrammar::setDumpFilename(const QString &path)
{
    _dumpFilename = path;
}

void CNFGrammar::dump() const
{
    ExtensionsSerialization::dumpToFile(_dumpFilename, *this);
}

void CNFGrammar::loadFromDump()
{
    ExtensionsSerialization::loadFromDump(_dumpFilename, *this);
}

QStringList CNFGrammar::getNonterminals(const QString &first)
{
    QStringList nonterminals;
    QMap<QString, const RuleCNFGrammar *>::iterator i(_mappingFirstToRules.find(first));

    while (i != _mappingFirstToRules.end() && i == first) {
        nonterminals.append(i.value()->_left);
        ++i;
    }

    return nonterminals;
}

QStringList CNFGrammar::getNonterminals(const QString &first, const QString &second)
{
    QStringList nonterminals;
    QString key = concatTerminals(first, second);
    QMap<QString, const RuleCNFGrammar *>::iterator i(_mappingRightToRules.find(key));

    while (i != _mappingRightToRules.end() && i == key) {
        nonterminals.append(i.value()->_left);
        ++i;
    }

    return nonterminals;
}

QString CNFGrammar::concatTerminals(const QString &first, const QString &second)
{
    return first + '\|' + second;
}

void CNFGrammar::clearCache()
{
    _mappingLeftToRules.clear();
    _mappingFirstToRules.clear();
    _mappingSecondToRules.clear();
    _mappingRightToRules.clear();
}

void CNFGrammar::fillCache(RuleCNFGrammar *p_rule)
{
    _mappingLeftToRules.insert(p_rule->_left, p_rule);
    _mappingFirstToRules.insert(p_rule->_first, p_rule);
    _mappingSecondToRules.insert(p_rule->_second, p_rule);
    _mappingRightToRules.insert(concatTerminals(p_rule->_first, p_rule->_second), p_rule);
}

QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr)
{
    ds << gr._rules;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr)
{
    if (!gr._rules.isEmpty()) {
        qWarning("Trying to deserialize CNFGrammar which is not empty (Tip: call clear() )");
        return ds;
    }

    ds >> gr._rules;
    for (int i = 0; i < gr._rules.size(); ++i) {
        gr.fillCache(&gr._rules[i]); // fills mapping
    }

    return ds;
}
