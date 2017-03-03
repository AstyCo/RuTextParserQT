#include "cnfgrammar.h"

#include <QFile>

CNFGrammar::CNFGrammar()
    : _dumpFilename("CNFGrammar.dump")
{
}

void CNFGrammar::addRule(const RuleCNFGrammar &rule)
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
    QFile file(_dumpFilename);

    if (!file.open(QFile::WriteOnly)) {
        qWarning(QString("error on openning file \'%1\' - dump failed").arg(_dumpFilename).toLocal8Bit().data());
        return;
    }

    QDataStream ds(&file);

    ds << *this;
}

void CNFGrammar::loadFromDump()
{
    QFile file(_dumpFilename);

    if (!file.open(QFile::ReadOnly)) {
        qWarning(QString("error on openning file \'%1\' - load from dump failed").arg(_dumpFilename).toLocal8Bit().data());
        return;
    }

    QDataStream ds(&file);

    ds >> *this;
}

void CNFGrammar::clearCache()
{
    _mappingLeftToRules.clear();
    _mappingFirstToRules.clear();
    _mappingSecondToRules.clear();
}

void CNFGrammar::fillCache(RuleCNFGrammar *p_rule)
{
    _mappingLeftToRules.insert(p_rule->_left, p_rule);
    _mappingFirstToRules.insert(p_rule->_first, p_rule);
    _mappingSecondToRules.insert(p_rule->_second, p_rule);
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
