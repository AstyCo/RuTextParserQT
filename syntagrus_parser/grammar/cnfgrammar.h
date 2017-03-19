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

    QVector<Scored> _rootScore;  ///< [featureID]-><root score>
    QVector<ScoredChomskyRuleRecord> _ruleByID; ///< [ruleID]->RuleRecord
    QVector<ListScoredListRuleID> _rulesByFeatureID;   ///< [featureID]->(ListOf(ListOf(<ruleID | featureID->X Y>), <number of occuriences>))

    // hash
    QVector<QVector<ListScoredRuleID> > _rulesByRightIDsHash;     ///< [leftFeatureID][rightFeatureID]->ListOf(<ruleID | X->leftFeatureID rightFeatureID>)
public:
    CNFGrammar();   ///< only for deserialization
    CNFGrammar(const featureID &featureCount);
    ~CNFGrammar();

    /*!
     * \brief Adds rule to grammar
     * \param rule Grammar rule in chomsky normal form
     */
    void append(const RuleRecord &rule);
    /*!
     * \brief Adds root rule to grammar
     * \param fid featureID of _root->featureID rule
     */
    void addRoot(const featureID &fid);
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
    inline const QVector<ScoredChomskyRuleRecord> &rulesByID() const;
    inline const QVector<ListScoredListRuleID> &rulesByFeatureID() const;

private:
    void initDumpFilename();
    void clearCache();
    void fillCache(const int &newID );
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

inline const QVector<ScoredChomskyRuleRecord> &CNFGrammar::rulesByID() const { return _ruleByID;}

inline const QVector<ListScoredListRuleID> &CNFGrammar::rulesByFeatureID() const { return _rulesByFeatureID;}

/// SERIALIZATION / DESERIALIZATION
QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr);
QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr);

#endif // CNFGRAMMAR_H
