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

HEADERS += \
    Application.h \
    MainWindow.h \
    NavigationManager.h

SOURCES += \
    main.cpp \
    Application.cpp \
    MainWindow.cpp \
    NavigationManager.cpp

RESOURCES += \
    $$PWD/../../resources/qml.qrc

DISTFILES += \
    $$PWD/qml/main.qml \
    $$PWD/qml/AppNavigation.qml \
    $$PWD/qml/LoginPage.qml
