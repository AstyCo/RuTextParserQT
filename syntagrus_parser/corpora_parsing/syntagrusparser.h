#ifndef SYNTAGRUSPARSER_H
#define SYNTAGRUSPARSER_H

#include "syntagrus_parser_global.h"
#include "treecorpora.h"

#include <QMap>
#include <QString>
#include <QDomDocument>

/*!
 * \brief The SynTagRusParser class parsing SynTagRus corpora into CNFGrammar
 *
 */
class SYNTAGRUS_PARSERSHARED_EXPORT SynTagRusParser
{
    static QString _debug;
    QString _synTagRusDirectory;
    QString _dumpFilenameTreeCorpora;

    TreeCorpora _treeCorpora;
public:
    SynTagRusParser();
    ~SynTagRusParser();

    /*!
     * \brief Parsing SynTagRus
     *  Function parsing all .tgt files in the setted by call of setDirectory(<path>),
     *  into the inner representation.
     */
    void parse(const QString &path = QString());

    TreeCorpora *getTreeCorpora();

    /*!
     * \brief Sets the directory with SynTagRusFiles
     * \param path Directory Path
     */
    void setDirectory(const QString &path);

    void serializeTreeCorpora() const;
    void deserializeTreeCorpora();

private:
    void parseXml(const QString &path);
    SentenceInCorpora parseSentence(const QDomElement &sentenceElement);
    void addByDom(SentenceInCorpora *sentence, int dom, QMap<int, RecordInCorpora *> &idToRecord);
};

#endif // SYNTAGRUSPARSER_H
