#ifndef DUMPABLE_H
#define DUMPABLE_H

#include "rutextparser_extensions_global.h"
#include "rutextparser_extensions.h"
#include <QDataStream>
#include <QString>

#define EXT_DECLARE_SERIALIZATION \
    void dump() const { ExtensionsSerialization::dumpToFile(_dumpFilename, *this);}\
    void load() { ExtensionsSerialization::loadFromDump(_dumpFilename, *this);}



class RUTEXTPARSER_EXTENSIONSSHARED_EXPORT Dumpable
{
protected:
    QString _dumpFilename;
public:
    Dumpable(const QString &filename);

    virtual void dump() const = 0;
    virtual void load() = 0;

    void setFilename(const QString &filename) { _dumpFilename = filename;}
    QString dumpFilename() const { return _dumpFilename;}
};

inline QDataStream &operator<<(QDataStream &ds, const Dumpable &s)
{
    Q_ASSERT_X(false,"QDataStream &operator<<(QDataStream &ds, const Dumpable &/*s*/)",
               toWarning(s.dumpFilename()));
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, Dumpable &s)
{
    Q_ASSERT_X(false,"QDataStream &operator<<(QDataStream &ds, const Dumpable &/*s*/)",
               toWarning(s.dumpFilename()));
    return ds;
}

#endif // DUMPABLE_H
