#ifndef MORPH_CLIENT_GLOBAL_H
#define MORPH_CLIENT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MORPH_CLIENT_LIBRARY)
#  define MORPH_CLIENTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MORPH_CLIENTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MORPH_CLIENT_GLOBAL_H
