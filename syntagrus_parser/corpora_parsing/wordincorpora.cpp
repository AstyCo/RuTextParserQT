#include "wordincorpora.h"

const QString &WordInCorpora::word() const
{
    return _word;
}

const QString &WordInCorpora::feature() const
{
    return _feature;
}

const QList<SintRel> &WordInCorpora::links() const
{
    return _links;
}

QList<SintRel> WordInCorpora::leftLinks() const
{
    QList<SintRel> res;
    foreach(const SintRel&link, _links) {
        if (link.first < _index)
            res.append(link);
    }
    return res;
}

QList<SintRel> WordInCorpora::rightLinks() const
{
    QList<SintRel> res;
    foreach(const SintRel&link, _links) {
        if (link.first > _index)
            res.append(link);
    }
    return res;
}

void WordInCorpora::append(const SintRel &link)
{
    _links.append(link);
}

bool WordInCorpora::isValid() const
{
    return _isValid;
}

WordInCorpora::WordInCorpora()
{
    _isValid = true;
}

WordInCorpora::WordInCorpora(const QString &feature, const SintRelIndex &index, const QString &word, const QList<SintRel> &links)
    : _index(index),
      _word(word),
      _feature(feature),
      _links(links)
{
    _isValid = true;
}

QDataStream &operator<<(QDataStream &ds, const WordInCorpora &w)
{
    ds << w._index;
    ds << w._word;
    ds << w._feature;
    ds << w._links;
    ds << w._isValid;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, WordInCorpora &w)
{
    ds >> w._index;
    ds >> w._word;
    ds >> w._feature;
    ds >> w._links;
    ds >> w._isValid;

    return ds;
}

SintRel makeSintRel(const SintRelIndex &index, const QString &sintRel)
{
    return qMakePair(index, sintRel);
}
