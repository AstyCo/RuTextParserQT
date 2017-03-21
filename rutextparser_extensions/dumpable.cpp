#include "dumpable.h"
#include "rutextparser_extensions.h"

Dumpable::Dumpable(const QString &filename)
{
#ifdef RUTEXTPARSERQT_DIR
    _dumpFilename = RUTEXTPARSERQT_DIR + filename;
#else
    qWarning("RUTEXTPARSERQT_DIR not defined");

    QString manualPath = "../../RuTextParserQT/";
    _dumpFilename = manualPath + filename;
#endif
}

void Dumpable::dump()
{
    ExtensionsSerialization::dumpToFile(_dumpFilename, *this);
}

void Dumpable::load()
{
    ExtensionsSerialization::loadFromDump(_dumpFilename, *this);
}


