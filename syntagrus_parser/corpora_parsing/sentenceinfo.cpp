#include "sentenceinfo.h"

#include "rutextparser_extensions.h"

SentenceInfo::SentenceInfo()
{

}

const WordInCorpora &SentenceInfo::operator[](int i) const
{
    Q_ASSERT_X(i>=0 && i<_vectorInfos.size(), "SentenceInfo::operator[]",
               toWarning("array out of index [%1] of size=%2", QString::number(i), QString::number(size())));

    return _vectorInfos[i];
}

WordInCorpora &SentenceInfo::operator[](int i)
{
    Q_ASSERT_X(i>=0 && i<_vectorInfos.size(), "SentenceInfo::operator[]",
               toWarning("array out of index [%1] of size=%2", QString::number(i), QString::number(size())));

    return _vectorInfos[i];
}

QStringList SentenceInfo::wordList() const
{
    QStringList result;
    foreach (const WordInCorpora &word, _vectorInfos) {
        result.append(word.word());
    }
    return result;
}

QStringList SentenceInfo::featList() const
{
    QStringList result;
    foreach (const WordInCorpora &word, _vectorInfos) {
        result.append(word.feature());
    }
    return result;
}



QDataStream &operator<<(QDataStream &ds, const SentenceInfo &s)
{
    ds << s._vectorInfos;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, SentenceInfo &s)
{
    ds >> s._vectorInfos;

    return ds;
}
