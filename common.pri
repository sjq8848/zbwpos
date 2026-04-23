VERSION = 1.0.0

CONFIG += c++17

QT += core gui widgets sql network

DEFINES += \
    QT_DEPRECATED_WARNINGS \
    QT_NO_CAST_TO_ASCII \
    QT_STRICT_ITERATORS \
    APP_VERSION=\\\"$$VERSION\\\"

INCLUDEPATH += $$PWD/src

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/build/debug
} else {
    DESTDIR = $$PWD/build/release
}
