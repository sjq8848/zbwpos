#pragma once
#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #define ZBW_QT6 1
    #define ZBW_QT5 0
#else
    #define ZBW_QT6 0
    #define ZBW_QT5 1
#endif
