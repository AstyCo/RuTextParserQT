#include "optimizedsentence.h"
#include "uniquevector.h"
#include "rutextparser_extensions.h"

OptimizedSentence::OptimizedSentence()
{

}

OptimizedSentence::OptimizedSentence(const SentenceInCorpora &sentence,
                                     const UniqueVector<featureID, QString> &mapper)
{
    const QVector<WordInCorpora> &words = sentence.qDebugSentence().vector();

    _words.resize(words.size());

    for (int i=0; i < words.size(); ++i) {
        _words[i] = OptimizedWord::fromWord(words[i], mapper);
    }
}

void OptimizedSentence::append(const SentenceInCorpora &s)
{
    const QVector<WordInCorpora> &words = s.qDebugSentence().vector();

    for (int i=0; i < _words.size(); ++i) {
        _words[i].append(words[i]);
    }
}

OptimizedSentence fromSentence(const SentenceInCorpora &sentence,
                               const UniqueVector<featureID, QString> &mapper)
{
    OptimizedSentence res;
    res.words().clear();

    const QVector<WordInCorpora> &words = sentence.qDebugSentence().vector();

    res.words().resize(words.size());

    for (int i=0; i < words.size(); ++i) {
        res.words()[i] = OptimizedWord::fromWord(words[i], mapper);
    }

    return res;
}

bool OptimizedSentence::operator==(const OptimizedSentence &os)
{
    return _words == os._words;
}

char *OptimizedSentence::toKey() const
{
    if (_words.isEmpty())
        return NULL;
    const int length = _words.size() * 2;
    char *key = new char[length + 1];
    key[length] = '\0';
    for (int j=0; j < _words.size(); ++j) {
        const featureID &fid = _words[j].getFeature();
        static const int blockSize = 0x40;

        key[2*j] = fid / blockSize + 0x80;
        key[2*j + 1] = fid % blockSize + 1;
    }
    return key;
}

QStringList OptimizedSentence::getSentence(const int &index) const
{
    QStringList result;
    if (index < 0 || index >= size()) {
        printWarning("OptimizedSentence::getSentence: out of index");
        return result;
    }

    for (int i=0; i < _words.size(); ++i) {
        result.append(_words.at(i).words.at(index));
    }
    return result;
}

QDataStream &operator<<(QDataStream &ds, const OptimizedSentence &s)
{
   return  ds << s._words;
}

QDataStream &operator>>(QDataStream &ds, OptimizedSentence &s)
{
    return  ds >> s._words;
}

QDebug operator<<(QDebug d, const OptimizedSentence &s)
{
    d << "OptimizedSentence(";
    foreach (const OptimizedWord &w, s.words())
        d << w;
    d << ')';

    return d;
}
