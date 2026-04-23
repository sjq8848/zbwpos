include(../common.pri)
QT += testlib

TEMPLATE = app
TARGET = zbwpos_tests

SOURCES += \
    unit/test_database.cpp \
    unit/test_models.cpp
