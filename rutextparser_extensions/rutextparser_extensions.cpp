#include "rutextparser_extensions.h"


const char *toWarning(const QString &templateString, const QString &arg1, const QString &arg2, const QString &arg3)
{
    return templateString.arg(arg1).arg(arg2).arg(arg3).toLocal8Bit().data();
}

void printWarning(const QString &templateString, const QString &arg1, const QString &arg2, const QString &arg3)
{
    qWarning(toWarning(templateString, arg1, arg2, arg3));
}
