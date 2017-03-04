#include "recordincorpora.h"

RecordInCorpora::RecordInCorpora()
{

}

RecordInCorpora::RecordInCorpora(int dom, const QString &feat, int id,
                                 const QString &lemma, const QString &link, const QString &word, QString nodeType)
    : _dom(dom), _feat(feat), _id(id),
      _lemma(lemma), _link(link), _word(word),
      _nodeType(nodeType)
{

}

QDataStream &operator<<(QDataStream &ds, const RecordInCorpora &r)
{
    ds << r._dom;
    ds << r._feat;
    ds << r._id;
    ds << r._lemma;
    ds << r._link;
    ds << r._word;
    ds << r._nodeType;

    return ds;
}

QDataStream &operator>>(QDataStream &ds, RecordInCorpora &r)
{
    ds >> r._dom;
    ds >> r._feat;
    ds >> r._id;
    ds >> r._lemma;
    ds >> r._link;
    ds >> r._word;
    ds >> r._nodeType;

    return ds;
}
