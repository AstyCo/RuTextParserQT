#ifndef DUMPABLE_H
#define DUMPABLE_H

#include "rutextparser_extensions_global.h"

#include <QDataStream>
#include <QString>


class RUTEXTPARSER_EXTENSIONSSHARED_EXPORT Dumpable
{
    QString _dumpFilename;
public:
    Dumpable(const QString &filename);

    void dump();
    void load();

    void setFilename(const QString &filename) { _dumpFilename = filename;}
};

inline QDataStream &operator<<(QDataStream &ds, const Dumpable &/*s*/)
{
    Q_ASSERT(false);
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, Dumpable &/*s*/)
{
    Q_ASSERT(false);
    return ds;
}

#endif // DUMPABLE_H
