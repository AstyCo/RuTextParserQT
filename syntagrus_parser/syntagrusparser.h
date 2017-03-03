#ifndef SYNTAGRUSPARSER_H
#define SYNTAGRUSPARSER_H

#include "syntagrus_parser_global.h"
#include "cnfgrammar.h"

#include <QMap>
#include <QString>
#include <QDomDocument>

/*!
 * \brief The SynTagRusParser class parsing SynTagRus corpora into CNFGrammar
 *
 */
class SYNTAGRUS_PARSERSHARED_EXPORT SynTagRusParser
{
    CNFGrammar *_grammar;
    QString _synTagRusDirectory;

public:
    SynTagRusParser();
    ~SynTagRusParser();

    /*!
     * \brief Parsing SynTagRus
     * Function parsing all .tgt files in the setted by setDirectory(<path>) function
     */
    void parse(const QString &path = QString());

    /*!
     * \brief Call it after parse() to get resulting Grammar
     * \return Pointer to <Grammar in Chomsky Normal Form>
     */
    CNFGrammar *getGrammar();

    /*!
     * \brief Sets the directory with SynTagRusFiles
     * \param path Directory Path
     */
    void setDirectory(const QString &path);

private:
    void parseXml(const QString &path);
    void parseSequence(const QDomElement &sequenceElement, const QString &tmp);
    static void insertRule(QMap<int, QStringList> *rules, int dom, QString link, QString feature);
    void addCNFRules(QMap<int, QStringList>::Iterator &i, const QMap<int, QString> &idToFeature);
    QString generateUniqueName() const;

    void generationMethod1(QMap<int, QStringList>::Iterator &i, const QMap<int, QString> &idToFeature);
};

#endif // SYNTAGRUSPARSER_H
