#ifndef SSCC_GLOBAL_H
#define SSCC_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore>

#if defined(SCREENSPACECAMERACONTROLLER_LIBRARY)
#  define CONTROLLER_EXPORT Q_DECL_EXPORT
#else
#  define CONTROLLER_EXPORT Q_DECL_IMPORT
#endif

#endif // SSCC_GLOBAL_H
