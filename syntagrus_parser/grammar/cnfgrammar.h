#ifndef CNFGRAMMAR_H
#define CNFGRAMMAR_H

#include "syntagrus_parser_global.h"

#include "rulecnfgrammar.h"

#include <QDataStream>
#include <QVector>
#include <QMap>
#include <QHash>

class SYNTAGRUS_PARSERSHARED_EXPORT CNFGrammar
{
    QString _dumpFilename;
public:
    CNFGrammar();
    ~CNFGrammar();

    /*!
     * \brief Adds rule to grammar
     * \param rule Grammar rule in chomsky normal form
     */
    void append(RuleCNFGrammar *rule);

    /*!
     * \brief Clears grammar
     */
    void clear();

    /*!
     * \brief Installs filename for dump-file
     * \param path Short filename
     */
    void setDumpFilename(const QString &path);

    /*!
     * \brief Dumps grammar to file
     * Use setDumpFilename(<path>) to set filename
     */
    void dump() const;

    /*!
     * \brief Loads CNFGrammar from CNFGrammar.dump (name may be different)
     */
    void loadFromDump();

    inline bool isEmpty() const;
    inline int size() const;
    inline const QVector<RuleCNFGrammar *> &rules() const;

    NonterminalList getNonterminals(const Terminal &terminal) const;
    NonterminalList getNonterminals(const Nonterminal &first, const Nonterminal &second) const;
    NonterminalList getNonterminals(const NonterminalList &firsts, const NonterminalList &seconds) const;

    QStringList toSentence(const QVector<NonterminalList> &row) const;

private:
    static NonterminalPair concatTerminals(const Nonterminal &first, const Nonterminal &second);
    void clearCache();
    void fillCache(RuleCNFGrammar *rule);

private:
    QVector<RuleCNFGrammar*> _rules;

    QMultiHash<Nonterminal, const RuleCNFGrammar *> _mappingLeftToRules;

    QMultiHash<Nonterminal, const RuleCNFGrammar *> _mappingFirstToRules;
    QMultiHash<Nonterminal, const RuleCNFGrammar *> _mappingSecondToRules;
    QMultiHash<NonterminalPair, const RuleCNFGrammar *> _mappingRightToRules;

    QMultiHash<Terminal, const RuleCNFGrammar *> _mappingTerminalToRules;

    /*!
     * Uses for serialization
     */
    friend QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr);
    friend QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr);
};

/// INLINE FUNCTIONS
inline bool CNFGrammar::isEmpty() const { return _rules.isEmpty();}

inline int CNFGrammar::size() const { return _rules.size();}

inline const QVector<RuleCNFGrammar*> &CNFGrammar::rules() const { return _rules;}

/// SERIALIZATION / DESERIALIZATION
QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr);
QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr);

#endif // CNFGRAMMAR_H
