#include "syntagrusparser.h"

#include "rutextparser_extensions.h"

#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QPair>
#include <QDirIterator>

#include <QDebug>

QString SynTagRusParser::_debug;

SynTagRusParser::SynTagRusParser()
{
    static QString directory("syntagrus_040712_DB");
    static QString dumpFilenameTreeCorpora("dumps/treecorpora.dump");

#ifdef RUTEXTPARSERQT_DIR
    _synTagRusDirectory = RUTEXTPARSERQT_DIR + directory;
    _dumpFilenameTreeCorpora = RUTEXTPARSERQT_DIR + dumpFilenameTreeCorpora;
#else
    qWarning("RUTEXTPARSERQT_DIR not defined");

    QString manualPath = "../../RuTextParserQT/";
    _synTagRusDirectory = manualPath + directory;
    _dumpFilenameTreeCorpora = manualPath + dumpFilenameTreeCorpora;
#endif

}

SynTagRusParser::~SynTagRusParser()
{
}

void SynTagRusParser::parse(const QString &path)
{
    _treeCorpora.clear();   // clear before parse

    QString synTagRusPath =  ( path.isEmpty()
                                 ? _synTagRusDirectory
                                 : path );

    QDirIterator it(synTagRusPath, QStringList() << "*.tgt", QDir::Files, QDirIterator::Subdirectories);
    int kk = 0;
    while (it.hasNext()) {
//        if (kk > 10)
//            return;

        parseXml(it.next());
        if (kk++ % 100 == 0) {
            qDebug() << QString("[%1] files parsed").arg(QString::number(kk));
            qDebug() << "\tlast one:" << it.filePath();
        }
    }
    qDebug() << "PARSING FINISHED";
}

TreeCorpora *SynTagRusParser::getTreeCorpora()
{
    return &_treeCorpora;
}

void SynTagRusParser::setDirectory(const QString &path)
{
    _synTagRusDirectory = path;
}

void SynTagRusParser::serializeTreeCorpora() const
{
    ExtensionsSerialization::dumpToFile(_dumpFilenameTreeCorpora, _treeCorpora);

    qDebug() << "_treeCorpora serialized";
}

void SynTagRusParser::deserializeTreeCorpora()
{
    _treeCorpora.clear();

    ExtensionsSerialization::loadFromDump(_dumpFilenameTreeCorpora, _treeCorpora);

    qDebug() << "_treeCorpora deserialized";
}

void SynTagRusParser::parseXml(const QString &path)
{
    QFile file(path);
    if (!file.exists()) {
        qCritical(QString("File %1 doesn't exist").arg(path).toLocal8Bit().data());
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical(QString("Can not open %1").arg(path).toLocal8Bit().data());
        return;
    }
    QDomDocument doc;

    if (!doc.setContent(&file)) {
        qWarning(QString("error on reading %1").arg(path).toLocal8Bit().data());
        file.close();
        return;
    }
    file.close();

    QDomElement bodyElem = doc.documentElement().firstChildElement("body");
    while (!bodyElem.isNull()){
        TextInCorpora textItem;
        QDomElement sentenceElem =  bodyElem.firstChildElement("S");
        while (!sentenceElem.isNull()) {
            _debug = path + ':' + sentenceElem.attribute("ID");

            SentenceInCorpora sentence = parseSentence(sentenceElem);
            if (sentence.isValid()) {
                textItem.append(sentence);
            }
            else {
                QString errMsg("Error in sentence [%1]\nError message: \'%2\'");
                qWarning(errMsg.arg(_debug).arg(sentence.error()).toLocal8Bit().data());
            }

            sentenceElem = sentenceElem.nextSiblingElement("S");
        }

        _treeCorpora.append(textItem);

        bodyElem = bodyElem.nextSiblingElement("body");    // go for all <body>
    }
}

SentenceInCorpora SynTagRusParser::parseSentence(const QDomElement &sentenceElement)
{
    SentenceInCorpora resultSentence;

    QMultiMap<int, RecordInCorpora > domToRecord;

    QDomElement wordElement = sentenceElement.firstChildElement("W");
    while (!wordElement.isNull()) {
        QDomAttr domAttr = wordElement.attributeNode("DOM");
        int dom;

        // get id
        if (domAttr.value() == "_root")
            dom = -1;
        else
            dom = domAttr.value().toInt();

        if (dom == 0){
            resultSentence.setError(QString("DOM attribute conversion error %1").arg(domAttr.value()));

            wordElement = wordElement.nextSiblingElement("W");
            continue;
        }

        int id = wordElement.attributeNode("ID").value().toInt();
        if (id == 0) {
            resultSentence.setError(QString("ID attribute conversion error %1").arg(wordElement.text()).toLocal8Bit().data());

            wordElement = wordElement.nextSiblingElement("W");
            continue;
        }
        QString feature = wordElement.attributeNode("FEAT").value();
        if (feature.isEmpty()) {
            if (wordElement.attribute("NODETYPE") != "FANTOM")
                resultSentence.setError(QString("FEAT attribute error %1").arg(_debug).toLocal8Bit().data());

            resultSentence.setSkip();
            wordElement = wordElement.nextSiblingElement("W");
            continue;
        }

        QString link = wordElement.attributeNode("LINK").value();
        QString lemma = wordElement.attributeNode("LEMMA").value();
        QString word = wordElement.text();


        domToRecord.insert(dom, RecordInCorpora(dom, feature, id, lemma, link, word));

        // make step
        wordElement = wordElement.nextSiblingElement("W");

        resultSentence.append(feature);
    }

    // filling sentence by records

    addByDom(&resultSentence, -1, domToRecord);
    return resultSentence;
}

void SynTagRusParser::addByDom(SentenceInCorpora *sentence, int dom, QMultiMap<int, RecordInCorpora> &domToRecord)
{
    int countOfRoot = 0;
    QMultiMap<int, RecordInCorpora>::const_iterator i = domToRecord.constFind( dom );
//    int count = 0;
    while(i != domToRecord.constEnd() && i.key() == dom) {
//        qDebug() << "count" << ++count;
//        qDebug() << QString("dom[%1] word[%2] itsDom(%3)")
//                    .arg(QString::number(dom))
//                    .arg(i.value()._word)
//                    .arg(i.value()._dom);

        if (dom == -1) {   // set root
            sentence->setRoot(new RecordNode(i.value()));

            if(++countOfRoot !=1) {
                sentence->setError(QString("found more than one _root DOM %1").arg(_debug));
                return;
            }
        }
        else {      // insert to node with nodeID
            RecordNode *node = sentence->nodeById(dom);
            if (!node) {
                sentence->setError(QString("failed to find node by ID %1").arg(_debug));
                return;
            }

            node->append(new RecordNode(i.value()));
        }

        // call this recursively
        addByDom(sentence, i.value()._id, domToRecord);

        ++i;
    }
}
