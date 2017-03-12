#include "treecorpora.h"


void TreeCorpora::append(const SentenceInCorpora &sentence)
{
    _sentencesBySize.insert(sentence.size(), sentence);

    foreach (const WordInCorpora &word, sentence.qDebugSentence().vector()) {
        if (_features.contains(word.feature()))
            continue;

        _features.append(word.feature());
    }
}

void TreeCorpora::append(const TextInCorpora &text)
{
    foreach (const SentenceInCorpora &sentence, text) {
        append(sentence);
    }
}

QDataStream &operator<<(QDataStream &ds, const TreeCorpora &t)
{
    ds << t._sentencesBySize;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, TreeCorpora &t)
{
    ds >> t._sentencesBySize;

    return ds;
}
