#ifndef RECORDINCORPORA_H
#define RECORDINCORPORA_H

#include <QString>
#include <QDataStream>

struct RecordInCorpora
{
    int _dom;
    QString _feat;
    int _id;
    QString _lemma;
    QString _link;
    QString _word;
    QString _nodeType;


public:
    RecordInCorpora();
    RecordInCorpora(int dom, const QString &feat, int id,
                    const QString &lemma, const QString &link,
                    const QString &word, QString nodeType = QString());

    inline bool before(const RecordInCorpora &anotherRecord);
    inline bool after(const RecordInCorpora &anotherRecord);

    friend QDataStream &operator<<(QDataStream &ds, const RecordInCorpora &r);
    friend QDataStream &operator>>(QDataStream &ds, RecordInCorpora &r);
};

QDataStream &operator<<(QDataStream &ds, const RecordInCorpora &r);
QDataStream &operator>>(QDataStream &ds, RecordInCorpora &r);

/// INLINE FUNCTIONS

inline bool RecordInCorpora::before(const RecordInCorpora &anotherRecord) {
    return _id < anotherRecord._id;
}

inline bool RecordInCorpora::after(const RecordInCorpora &anotherRecord) {
    return _id > anotherRecord._id;
}

#endif // RECORDINCORPORA_H
