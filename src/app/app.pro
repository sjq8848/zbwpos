include(../../common.pri)
equals(QT_MAJOR_VERSION, 5): include(../../qt5_compat.pri)
equals(QT_MAJOR_VERSION, 6): include(../../qt6_compat.pri)
include(../core/core.pri)
include(../cashier/cashier.pri)
include(../product/product.pri)
include(../member/member.pri)
include(../../android/android.pri)

TARGET = zbwpos
TEMPLATE = app

SOURCES += \
    main.cpp

RESOURCES += \
    $$PWD/../../resources/qml.qrc
