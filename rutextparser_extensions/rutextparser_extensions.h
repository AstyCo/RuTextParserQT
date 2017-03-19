#ifndef RUTEXTPARSER_EXTENSIONS_H
#define RUTEXTPARSER_EXTENSIONS_H

#include "rutextparser_extensions_global.h"

#include <QFile>
#include <QDataStream>

RUTEXTPARSER_EXTENSIONSSHARED_EXPORT const char *toWarning(const QString &templateString );
RUTEXTPARSER_EXTENSIONSSHARED_EXPORT const char *toWarning(const QString &templateString,
                                                           const QString &arg1 );
RUTEXTPARSER_EXTENSIONSSHARED_EXPORT const char *toWarning(const QString &templateString,
                                                           const QString &arg1,
                                                           const QString &arg2 );
RUTEXTPARSER_EXTENSIONSSHARED_EXPORT const char *toWarning(const QString &templateString,
                                                           const QString &arg1,
                                                           const QString &arg2,
                                                           const QString &arg3 );

RUTEXTPARSER_EXTENSIONSSHARED_EXPORT void printWarning(const QString &templateString);
RUTEXTPARSER_EXTENSIONSSHARED_EXPORT void printWarning(const QString &templateString,
                                                       const QString &arg1 );
RUTEXTPARSER_EXTENSIONSSHARED_EXPORT void printWarning(const QString &templateString,
                                                       const QString &arg1,
                                                       const QString &arg2 );
RUTEXTPARSER_EXTENSIONSSHARED_EXPORT void printWarning(const QString &templateString,
                                                       const QString &arg1,
                                                       const QString &arg2,
                                                       const QString &arg3 );


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
