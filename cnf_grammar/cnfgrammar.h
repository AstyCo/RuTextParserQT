#ifndef CNFGRAMMAR_H
#define CNFGRAMMAR_H

#include "cnf_grammar_global.h"

#include "rulecnfgrammar.h"

#include <QDataStream>
#include <QVector>
#include <QMap>

class CNF_GRAMMARSHARED_EXPORT CNFGrammar
{
    QString _dumpFilename;
public:
    CNFGrammar();

    /*!
     * \brief Adds rule to grammar
     * \param rule Grammar rule in chomsky normal form
     */
    void addRule(const RuleCNFGrammar &rule);

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

    bool isEmpty() const;
    int size() const;
    const QVector<RuleCNFGrammar> &rules() const;

private:
    void clearCache();
    void fillCache(RuleCNFGrammar *rule);

private:
    QVector<RuleCNFGrammar> _rules;

    QMap<QString, const RuleCNFGrammar *> _mappingLeftToRules;
    QMap<QString, const RuleCNFGrammar *> _mappingFirstToRules;
    QMap<QString, const RuleCNFGrammar *> _mappingSecondToRules;

    /*!
     * Uses for serialization
     */
    friend QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr);
    friend QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr);
};

/// INLINE FUNCTIONS
inline bool CNFGrammar::isEmpty() const { return _rules.isEmpty();}

inline int CNFGrammar::size() const { return _rules.size();}

inline const QVector<RuleCNFGrammar> &CNFGrammar::rules() const { return _rules;}

/// SERIALIZATION / DESERIALIZATION
QDataStream &operator<<(QDataStream &ds, const CNFGrammar &gr);
QDataStream &operator>>(QDataStream &ds, CNFGrammar &gr);

#endif // CNFGRAMMAR_H
