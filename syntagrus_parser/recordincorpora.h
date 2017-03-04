#ifndef RECORDINCORPORA_H
#define RECORDINCORPORA_H

#include <QString>

struct RecordInCorpora
{
    int _dom;
    QString _feat;
    int _id;
    QString _lemma;
    QString _link;
    QString _word;

    enum NodeType {
        NoType,
        Fantom
    };

public:
    RecordInCorpora();
    RecordInCorpora(int dom, const QString &feat, int id,
                    const QString &lemma, const QString &link, const QString &word);

    bool before(const RecordInCorpora &anotherRecord);
    bool after(const RecordInCorpora &anotherRecord);
};

/// INLINE FUNCTIONS

inline bool RecordInCorpora::before(const RecordInCorpora &anotherRecord) {
    return _id < anotherRecord._id;
}

inline bool RecordInCorpora::after(const RecordInCorpora &anotherRecord) {
    return _id > anotherRecord._id;
}

#endif // RECORDINCORPORA_H
