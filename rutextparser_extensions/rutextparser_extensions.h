#ifndef RUTEXTPARSER_EXTENSIONS_H
#define RUTEXTPARSER_EXTENSIONS_H

#include "rutextparser_extensions_global.h"

#include <QFile>
#include <QDataStream>

namespace ExtensionsSerialization {

template <typename T>
void dumpToFile(const QString &filename, const T &o)
{
    QFile file(filename);

    if (!file.open(QFile::WriteOnly)) {
        qWarning(QString("error on openning file \'%1\' - dump failed").arg(filename).toLocal8Bit().data());
        return;
    }

    QDataStream ds(&file);

    ds << o;
}

template <typename T>
void loadFromDump(const QString &filename, T &o)
{
    QFile file(filename);

    if (!file.open(QFile::ReadOnly)) {
        qWarning(QString("error on openning file \'%1\' - load from dump failed").arg(filename).toLocal8Bit().data());
        return;
    }

    QDataStream ds(&file);

    ds >> o;
}

}   // End of namepsace Serialization

#endif // RUTEXTPARSER_EXTENSIONS_H
