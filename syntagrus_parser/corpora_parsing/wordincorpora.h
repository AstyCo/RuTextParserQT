#ifndef WORDINCORPORA_H
#define WORDINCORPORA_H

#include "syntagrus_parser_global.h"

#include <QString>
#include <QList>
#include <QPair>
#include <QDataStream>

typedef unsigned int uint;
typedef int SintRelIndex;
typedef QPair<SintRelIndex, QString> SintRel;

SintRel SYNTAGRUS_PARSERSHARED_EXPORT makeSintRel(const SintRelIndex &index, const QString &sintRel);

class SYNTAGRUS_PARSERSHARED_EXPORT WordInCorpora
{
    SintRelIndex _index;
    QString _word;
    QString _feature;
    QList<SintRel> _links;

    bool _isValid;

public:
    WordInCorpora();
    WordInCorpora(const QString &feature,
                  const SintRelIndex &index,
                  const QString &word = QString(),
                  const QList<SintRel> &links = QList<SintRel>());


    const QString &word() const;
    const QString &feature() const;

    const QList<SintRel> &links() const;
    QList<SintRel> leftLinks() const;
    QList<SintRel> rightLinks() const;

    void append(const SintRel &link);
    bool isValid() const;

    friend QDataStream &operator<<(QDataStream &ds, const WordInCorpora &w);
    friend QDataStream &operator>>(QDataStream &ds, WordInCorpora &w);

};

QDataStream &operator<<(QDataStream &ds, const WordInCorpora &w);
QDataStream &operator>>(QDataStream &ds, WordInCorpora &w);

#endif // WORDINCORPORA_H
