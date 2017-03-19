#ifndef CNFGRAMMAR_H
#define CNFGRAMMAR_H

#include "syntagrus_parser_global.h"
#include "internal/base-types.h"
#include "rulerecord.h"
#include "scoredrules.h"

#include <QDataStream>
#include <QVector>
#include <QMap>
#include <QHash>

class SYNTAGRUS_PARSERSHARED_EXPORT CNFGrammar
{
    QString _dumpFilename;

    QVector<ScoredChomskyRuleRecord> _ruleByID; ///< [ruleID]->RuleRecord
    QVector<ListScoredListRuleID> _rulesByFeatureID;   ///< [featureID]->(ListOf(ListOf(<ruleID | featureID->X Y>), <number of occuriences>))

    // hash
    QVector<QVector<ListScoredRuleID> > _rulesByRightIDsHash;     ///< [leftFeatureID][rightFeatureID]->ListOf(<ruleID | X->leftFeatureID rightFeatureID>)
public:
    CNFGrammar(const featureID &featureCount);
    ~CNFGrammar();

    /*!
     * \brief Adds rule to grammar
     * \param rule Grammar rule in chomsky normal form
     */
    void append(const RuleRecord &rule);

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
    ruleID insert(const ChomskyRuleRecord &rule);
    ruleID find(const ChomskyRuleRecord &rule) const;
    void insert(const featureID &srcRuleID, const ListRuleID &ids);
    int find(const featureID &srcRuleID, const ListRuleID &ids) const;

    /*!
     * Uses for serialization
     */
    friend QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr);
    friend QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr);
};

/// INLINE FUNCTIONS
inline bool CNFGrammar::isEmpty() const { return _ruleByID.isEmpty();}

inline int CNFGrammar::size() const { return _ruleByID.size();}

inline const QVector<RuleCNFGrammar*> &CNFGrammar::rules() const { return _ruleByID;}

/// SERIALIZATION / DESERIALIZATION
QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr);
QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr);

#endif // CNFGRAMMAR_H
