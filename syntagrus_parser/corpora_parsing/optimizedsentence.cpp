#include "optimizedsentence.h"

OptimizedSentence::OptimizedSentence()
{

}

OptimizedSentence::OptimizedSentence(const SentenceInCorpora &sentence, const QHash<QString, FeatureId> &hashFeatures)
{
    const QVector<WordInCorpora> &words = sentence.qDebugSentence().vector();

    _words.resize(words.size());

    for (int i=0; i < words.size(); ++i) {
        _words[i] = OptimizedWord::fromWord(words[i], hashFeatures);
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
                               const QHash<QString, FeatureId> &hashFeatures)
{
    OptimizedSentence res;
    res.words().clear();

    const QVector<WordInCorpora> &words = sentence.qDebugSentence().vector();

    res.words().resize(words.size());

    for (int i=0; i < words.size(); ++i) {
        res.words()[i] = OptimizedWord::fromWord(words[i], hashFeatures);
    }

    return res;
}

bool OptimizedSentence::operator==(const OptimizedSentence &os)
{
    return _words == os._words;
}
