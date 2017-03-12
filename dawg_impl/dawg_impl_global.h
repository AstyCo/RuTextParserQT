#ifndef DAWG_IMPL_GLOBAL_H
#define DAWG_IMPL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DAWG_IMPL_LIBRARY)
#  define DAWG_IMPLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DAWG_IMPLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DAWG_IMPL_GLOBAL_H
