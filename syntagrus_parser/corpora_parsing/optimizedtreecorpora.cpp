#include "optimizedtreecorpora.h"
#include "rutextparser_extensions.h"

OptimizedTreeCorpora::OptimizedTreeCorpora()
{
    initDumpFilename();
}

OptimizedTreeCorpora::OptimizedTreeCorpora(const TreeCorpora &tree)
    : _featureMapper(tree.featureMapper())
{
    initDumpFilename();

    QMultiHash<int, SentenceInCorpora>::const_iterator it(tree.sentencesBySize().constBegin());
    while( it != tree.sentencesBySize().constEnd()) {
        OptimizedSentence optimizedSentence(it.value(), _featureMapper);
        QMultiHash<int, OptimizedSentence>::iterator insertedSentence
                = _multihashSentences.find(it.key(), optimizedSentence);
        if (insertedSentence != _multihashSentences.end()) {
            QVector<OptimizedWord> &owords = insertedSentence.value().words();
            const QVector<WordInCorpora> &swords = it.value().qDebugSentence().vector();

            qDebug() << "EQUALS";
            for (int i=0; i<owords.size(); ++i) {
                qDebug() << swords[i].feature() << swords[i].word();
                qDebug() << owords[i].words.first();
            }

            insertedSentence.value().append(it.value());
        }
        else {
            _multihashSentences.insert(it.key(), optimizedSentence);
        }

        it++;
    }

}

void OptimizedTreeCorpora::serialize() const
{
    ExtensionsSerialization::dumpToFile(_dumpFilename, *this);

    qDebug() << "OptimizedTreeCorpora serialized";
}

void OptimizedTreeCorpora::deserialize()
{
    clear();

    ExtensionsSerialization::loadFromDump(_dumpFilename, *this);

    qDebug() << "OptimizedTreeCorpora deserialized";
}

void OptimizedTreeCorpora::clear()
{
    _featureMapper.clear();
    _multihashSentences.clear();
}

void OptimizedTreeCorpora::initDumpFilename()
{
    static QString dumpFilename("dumps/optimized_tree.dump");

#ifdef RUTEXTPARSERQT_DIR
    _dumpFilename = RUTEXTPARSERQT_DIR + dumpFilename;
#else
    qWarning("RUTEXTPARSERQT_DIR not defined");

    QString manualPath = "../../RuTextParserQT/";
    _dumpFilename = manualPath + dumpFilename;
#endif
}

QDataStream &operator<<(QDataStream &ds, const OptimizedTreeCorpora &o)
{
    ds << o._dumpFilename;
    ds << o._featureMapper;
    ds << o._multihashSentences;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, OptimizedTreeCorpora &o)
{
    ds >> o._dumpFilename;
    ds >> o._featureMapper;
    ds >> o._multihashSentences;

    return ds;
}

