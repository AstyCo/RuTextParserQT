#include "cnfgrammar.h"

#include "rutextparser_extensions.h"

#include <QFile>
#include <QStringList>

#include <QDebug>

QDebug operator<< (QDebug d, const RuleCNFGrammar &r) {
    if (r._type == RuleCNFGrammar::TerminalRule) {
        d << r._leftNonterminal
          << "->"
          << r._terminal;
    }
    else {
        d << r._leftNonterminal
          << "->"
          << r._firstNonterminal
          << ' '
          << r._secondNonterminal;
    }

    return d;
}


CNFGrammar::CNFGrammar()
    : _dumpFilename("CNFGrammar.dump")
{
}

CNFGrammar::~CNFGrammar()
{
    clear();
}

void CNFGrammar::append(RuleCNFGrammar *rule)
{
    _rules.append(rule);

//    qDebug() << "added"<< *rule << endl;

    fillCache(rule);
}

void CNFGrammar::clear()
{
    clearCache();

    foreach (RuleCNFGrammar* rule, _rules)
        delete rule;

    _rules.clear();
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

NonterminalList CNFGrammar::getNonterminals(const Terminal &terminal) const
{
    NonterminalList nonterminals;
    QMultiHash<Terminal, const RuleCNFGrammar *>::const_iterator
            i(_mappingTerminalToRules.constFind(terminal));

//    qDebug() << QString("looking for rule[X -> %1]").arg(terminal);
    while (i != _mappingTerminalToRules.constEnd() && i.key() == terminal) {
//        qDebug() << "terminal key" << i.key();
//        qDebug() << "terminal value" << *i.value();
        nonterminals.append(i.value()->_leftNonterminal);
        ++i;
    }

    return nonterminals;
}

NonterminalList CNFGrammar::getNonterminals(const Nonterminal &first, const Nonterminal &second) const
{
//    qDebug() << QString("looking for rule[X -> %1 %2]")
//                .arg(QString::number(first))
//                .arg(QString::number(second));

    NonterminalList nonterminals;
    NonterminalPair key = concatTerminals(first, second);
    QMultiHash<NonterminalPair, const RuleCNFGrammar *>::const_iterator i(_mappingRightToRules.find(key));

    while (i != _mappingRightToRules.constEnd() && i.key() == key) {
        nonterminals.append(i.value()->_leftNonterminal);
        ++i;
    }

    return nonterminals;
}

NonterminalList CNFGrammar::getNonterminals(const NonterminalList &firsts, const NonterminalList &seconds) const
{
    NonterminalList res;
    qDebug() << QString("getNonterminals fsz[%1] ssz[%2]")
                .arg(QString::number(firsts.size()))
                .arg(QString::number(seconds.size()));
//    qDebug() << "firsts" << firsts;
//    qDebug() << "seconds" << seconds;


    foreach (const Nonterminal &f, firsts)
        foreach (const Nonterminal &s, seconds) {
            res.append(getNonterminals(f, s));
        }

//    qDebug() << "res sz" << res.size();
    return res;
}

QStringList CNFGrammar::toSentence(const QVector<NonterminalList> &row) const
{
    QStringList res;
    foreach (const NonterminalList &nlist, row ){
//        qDebug() << "tosent";

        QMultiHash<Nonterminal, const RuleCNFGrammar *>::const_iterator i = _mappingLeftToRules.constFind(nlist.first());
        Q_ASSERT(i != _mappingLeftToRules.constEnd() && i.key() == nlist.first());
        QString feat = i.value()->_terminal;

        Q_ASSERT(!feat.isEmpty());
        res.append(feat);
    }
    return res;
}

NonterminalPair CNFGrammar::concatTerminals(const Nonterminal &first, const Nonterminal &second)
{
    return qMakePair(first, second);
}

void CNFGrammar::clearCache()
{
    _mappingLeftToRules.clear();

    _mappingFirstToRules.clear();
    _mappingSecondToRules.clear();
    _mappingRightToRules.clear();

    _mappingTerminalToRules.clear();;
}

void CNFGrammar::fillCache(RuleCNFGrammar *p_rule)
{
    _mappingLeftToRules.insert(p_rule->_leftNonterminal, p_rule);
    if (p_rule->isNonterminalRule()) {
        _mappingFirstToRules.insert(p_rule->_firstNonterminal, p_rule);
        _mappingSecondToRules.insert(p_rule->_secondNonterminal, p_rule);
        _mappingRightToRules.insert(
                    concatTerminals(p_rule->_firstNonterminal, p_rule->_secondNonterminal),
                    p_rule);
    }
    else {
        _mappingTerminalToRules.insert(p_rule->_terminal, p_rule);
    }
}

QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr)
{
    ds << gr._rules.size();
    for (int i = 0; i < gr._rules.size(); ++i)
        ds << *gr._rules[i];

    return ds;
}

QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr)
{
    if (!gr._rules.isEmpty()) {
        qWarning("Trying to deserialize CNFGrammar which is not empty (Tip: call clear() )");
        return ds;
    }

    int rulesSize;
    ds >> rulesSize;
    Q_ASSERT(rulesSize >= 0);

    gr._rules.resize(rulesSize);
    for (int i = 0; i < rulesSize; ++i){
        gr._rules[i] = new RuleCNFGrammar();
        ds >> *gr._rules[i];
    }
    for (int i = 0; i < gr._rules.size(); ++i) {
        gr.fillCache(gr._rules[i]); // fills mapping
    }

    return ds;
}
