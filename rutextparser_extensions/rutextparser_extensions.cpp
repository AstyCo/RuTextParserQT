#include "rutextparser_extensions.h"

const char *toWarning(const QString &templateString)
{
    return templateString.toLocal8Bit().data();
}

const char *toWarning(const QString &templateString, const QString &arg1)
{
    return templateString.arg(arg1).toLocal8Bit().data();
}

const char *toWarning(const QString &templateString, const QString &arg1, const QString &arg2)
{
    return templateString.arg(arg1).arg(arg2).toLocal8Bit().data();
}

const char *toWarning(const QString &templateString, const QString &arg1, const QString &arg2, const QString &arg3)
{
    return templateString.arg(arg1).arg(arg2).arg(arg3).toLocal8Bit().data();
}

void printWarning(const QString &templateString)
{
    qWarning(toWarning(templateString));
}

void printWarning(const QString &templateString, const QString &arg1)
{
    qWarning(toWarning(templateString, arg1));
}

void printWarning(const QString &templateString, const QString &arg1, const QString &arg2)
{
    qWarning(toWarning(templateString, arg1, arg2));
}


void printWarning(const QString &templateString, const QString &arg1, const QString &arg2, const QString &arg3)
{
    qWarning(toWarning(templateString, arg1, arg2, arg3));
}

bool ExtensionsLogs::Logs::registerLogStream(const QString &filename) {
    if (getInstance()->_registeredLogStreams.contains(filename))
        return true;

    LogStream *log = new LogStream(filename);
    if (!log->_success) {
        delete log;
        return false;
    }
    getInstance()->_registeredLogStreams.insert(filename, log);
    return true;
}

ExtensionsLogs::Logs *ExtensionsLogs::Logs::getInstance() {
    static Logs log;
    return &log;
}

ExtensionsLogs::LogStream::LogStream(const QString &fname)
    : file(fname)
{

    if (!file.open(QFile::WriteOnly)) {
        _success = false;
        return;
    }

    setDevice(&file);
    _success = true;
}
