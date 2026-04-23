lessThan(QT_MAJOR_VERSION, 5) | greaterThan(QT_MAJOR_VERSION, 6) {
    error("Unsupported Qt version. Use Qt 5.15 or Qt 6.x")
}

include(common.pri)

equals(QT_MAJOR_VERSION, 5) {
    include(qt5_compat.pri)
    message("Building with Qt 5.x")
}
equals(QT_MAJOR_VERSION, 6) {
    include(qt6_compat.pri)
    message("Building with Qt 6.x")
}

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    src

# tests are optional, build manually when needed
# tests
