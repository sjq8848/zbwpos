include(../common.pri)
QT += testlib

TEMPLATE = app
TARGET = zbwpos_tests

HEADERS += \
    unit/test_database.h \
    unit/test_models.h

SOURCES += \
    unit/test_database.cpp \
    unit/test_models.cpp
