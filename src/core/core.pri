HEADERS += \
    $$PWD/QtCompat.h \
    $$PWD/Common.h \
    $$PWD/database/DatabaseManager.h \
    $$PWD/database/Migration.h \
    $$PWD/database/Models.h \
    $$PWD/network/ApiClient.h \
    $$PWD/network/AuthManager.h \
    $$PWD/network/OfflineQueue.h \
    $$PWD/network/SyncManager.h \
    $$PWD/network/models/ApiResponse.h \
    $$PWD/network/endpoints/AuthApi.h \
    $$PWD/network/endpoints/ProductApi.h \
    $$PWD/network/endpoints/MemberApi.h \
    $$PWD/network/endpoints/OrderApi.h

SOURCES += \
    $$PWD/database/DatabaseManager.cpp \
    $$PWD/database/Migration.cpp \
    $$PWD/network/ApiClient.cpp \
    $$PWD/network/AuthManager.cpp \
    $$PWD/network/OfflineQueue.cpp \
    $$PWD/network/SyncManager.cpp \
    $$PWD/network/endpoints/AuthApi.cpp \
    $$PWD/network/endpoints/ProductApi.cpp \
    $$PWD/network/endpoints/MemberApi.cpp \
    $$PWD/network/endpoints/OrderApi.cpp
