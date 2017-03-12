#ifndef WORDINCORPORA_H
#define WORDINCORPORA_H

#include "syntagrus_parser_global.h"

#include <QString>
#include <QList>
#include <QPair>
#include <QDataStream>

typedef unsigned int uint;
typedef int WordIndex;
typedef QPair<WordIndex, QString> SintRel;
typedef QList<SintRel> SintRelList;

SintRel SYNTAGRUS_PARSERSHARED_EXPORT makeSintRel(const WordIndex &index, const QString &sintRel);

class SYNTAGRUS_PARSERSHARED_EXPORT WordInCorpora
{
    WordIndex _dom;
    WordIndex _index;
    QString _word;
    QString _feature;
    SintRelList _links;

    bool _isValid;

public:
    WordInCorpora();
    WordInCorpora(const QString &feature,
                  const WordIndex &dom,
                  const WordIndex &index,
                  const QString &word = QString(),
                  const QList<SintRel> &links = QList<SintRel>());

    const WordIndex &dom() const { return _dom;}
    const WordIndex &index() const { return _index;}
    const SintRelList &links() const { return _links;}
    const QString &word() const;
    const QString &feature() const;

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
