#ifndef RUTEXTPARSER_EXTENSIONS_H
#define RUTEXTPARSER_EXTENSIONS_H

#include "rutextparser_extensions_global.h"

#include <QFile>
#include <QDataStream>
#include <QTextStream>

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

namespace ExtensionsLogs {
class RUTEXTPARSER_EXTENSIONSSHARED_EXPORT LogStream : public QTextStream
{
    QFile file;

    bool _success;
public:
    explicit LogStream(const QString &fname);

    friend class Logs;
};

class RUTEXTPARSER_EXTENSIONSSHARED_EXPORT Logs
{
    QMap<QString, LogStream *> _registeredLogStreams;
public:
    static bool registerLogStream(const QString &filename);
    static LogStream *log(const QString &filename) { return getInstance()->_registeredLogStreams.value(filename, NULL);}
    static Logs *getInstance();

private:
    Logs() {}
    Q_DISABLE_COPY(Logs);
};

}

namespace ExtensionsQtContainers {


template <typename T>
inline bool compareBySize(const T &lhs, const T &rhs) {
    return lhs.size() < rhs.size();
}

template <typename T, typename TValue>
typename T::iterator insert_sorted(T &containerRA, const TValue &value) {
    return containerRA.insert
            (
                std::upper_bound( containerRA.begin(), containerRA.end(), value ),
                value
            );
}

template <typename T>
typename QList<T>::iterator insert_sorted(QList<T> &list, const T &value) {
    return list.insert
            (
                std::upper_bound( list.begin(), list.end(), value ),
                value
            );
}
}


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
