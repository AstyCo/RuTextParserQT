#include "treecorpora.h"


void TreeCorpora::append(const SentenceInCorpora &sentence)
{
    _sentencesBySize.insert(sentence.size(), sentence);

    foreach (const WordInCorpora &word, sentence.qDebugSentence().vector()) {
        if (_featureMapper.index(word.feature()) == -1) {
            _featureMapper.append(word.feature());
        }
        foreach (const SintRel &sintRel, word.links()) {
            if (_linkMapper.index(sintRel.second) == INVALID_LINK)
                _linkMapper.append(sintRel.second);
        }
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
    t._featureMapper.dump();
    t._linkMapper.dump();

    return ds;
}

QDataStream &operator>>(QDataStream &ds, TreeCorpora &t)
{
    ds >> t._sentencesBySize;
    t._featureMapper.load();
    t._linkMapper.load();

    return ds;
}
