include(../common.pri)
QT += testlib

TEMPLATE = app
TARGET = zbwpos_tests

SOURCES += \
    unit/test_database.cpp \
    unit/test_models.cpp \
    integration/test_full_cashier_flow.cpp \
    integration/test_sync_flow.cpp
