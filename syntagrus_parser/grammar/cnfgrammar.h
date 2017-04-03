#ifndef CNFGRAMMAR_H
#define CNFGRAMMAR_H

#include "syntagrus_parser_global.h"
#include "internal/base-types.h"
//#include "rutextparser_extensions.h"
#include "corpora_parsing/recordnode.h"
#include "simpleruletree.h"
#include "rulerecord.h"
#include "scoredrules.h"

#include <QDataStream>
#include <QVector>
#include <QMap>
#include <QHash>

class SYNTAGRUS_PARSERSHARED_EXPORT CNFGrammar
{
    QString _dumpFilename;
    featureID _size;

    QVector<Scored> _rootScore;  ///< [featureID]-><root score>
    QVector<ScoredChomskyRuleRecord> _ruleByID; ///< [ruleID]->RuleRecord

    QVector<SimpleRuleTree> _rulesByFeatureID;   ///< [featureID]->(SetOf(SetOf(<ruleID | featureID->X Y>), <number of occuriences>))
    SimpleRuleTree _conseqRules;   ///< Reversed records from corpora depends with length of 2

    // hash
    QVector<QVector<ListRuleID> > _rulesByRightIDsHash;     ///< [leftFeatureID][rightFeatureID]->ListOf(<ruleID | X->leftFeatureID rightFeatureID>)
//    QVector<ListScoredListRuleID> *_plistRulesByFID;
public:
    CNFGrammar();   ///< only for deserialization
    CNFGrammar(const featureID &featureCount);
    ~CNFGrammar();

//    void startBuilding() { _plistRulesByFID = new QVector<ListScoredListRuleID>(_size);}
//    void finishBuilding();

    QString toReport(const FeatureMapper &fmapper) const;

    void append(const UniqueVector<featureID, QString> &fmapper,
                const UniqueVector<linkID, QString> &lmapper,
                const RecordNode *node,
                const ruleID &parentID);
    /*!
     * \brief Adds rule to grammar
     * \param rule Grammar rule in chomsky normal form
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
    inline const QVector<Scored> &rootScore() const;
    inline const QVector<ScoredChomskyRuleRecord> &rulesByID() const;
    inline const QVector<SimpleRuleTree> &rulesByFeatureID() const;
    inline const SimpleRuleTree &conseqRules() const;
    inline const QVector<QVector<ListRuleID> > &rulesByRightIDsHash() const;
    ListRuleID produceSequenceForCYK(const ListRuleID &ids) const;
    ListRuleID fromCYK(const ListRuleID &ids) const;
private:
    void resizeVectors(int sz);
    void resizeMatrix(int sz);
    void initDumpFilename();
    void clearCache();
    void fillCache(const int &newID );
    ruleID insert(const ChomskyRuleRecord &rule);
    ListRuleID::const_iterator find(const ChomskyRuleRecord &rule) const;
    void insertWidthRule(const featureID &srcRuleID, const ListRuleID &ids, bool rootRule);
    void insertInDepthRule(const ruleID &rid, const ruleID &prid);
//    QSet::iterator find(const featureID &srcRuleID, const SetRuleID &ids);

    /*!
     * Uses for serialization
     */
    friend QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr);
    friend QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr);

    Q_DISABLE_COPY(CNFGrammar);
};

/// INLINE FUNCTIONS
inline bool CNFGrammar::isEmpty() const { return _ruleByID.isEmpty();}

inline int CNFGrammar::size() const { return _ruleByID.size();}

inline const QVector<ScoredChomskyRuleRecord> &CNFGrammar::rulesByID() const { return _ruleByID;}

inline const QVector<SimpleRuleTree> &CNFGrammar::rulesByFeatureID() const { return _rulesByFeatureID;}

inline const SimpleRuleTree &CNFGrammar::conseqRules() const { return _conseqRules;}

inline const QVector<Scored> &CNFGrammar::rootScore() const { return _rootScore;}

inline const QVector<QVector<ListRuleID> > &CNFGrammar::rulesByRightIDsHash() const { return _rulesByRightIDsHash;}

/// SERIALIZATION / DESERIALIZATION
QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr);
QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr);

#endif // CNFGRAMMAR_H
