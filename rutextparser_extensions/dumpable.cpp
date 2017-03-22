#include "dumpable.h"
#include "rutextparser_extensions.h"

#include <QDebug>

Dumpable::Dumpable(const QString &filename)
{
#ifdef RUTEXTPARSERQT_DIR
    _dumpFilename = RUTEXTPARSERQT_DIR + QString("dumps/") + filename;
#else
    qWarning("RUTEXTPARSERQT_DIR not defined");

    QString manualPath = "../../RuTextParserQT/dumps/";
    _dumpFilename = manualPath + filename;
#endif
}

//void Dumpable::dump() const
//{
//    ExtensionsSerialization::dumpToFile(_dumpFilename, *this);

//    qDebug() << _dumpFilename << "dumped";
//}

//void Dumpable::load()
//{
//    ExtensionsSerialization::loadFromDump(_dumpFilename, *this);

//    qDebug() << _dumpFilename << "loaded";
//}


