#include "treecorpora.h"


void TreeCorpora::append(const SentenceInCorpora &text)
{
    _sentencesBySize.insert(text.size(), text);
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
