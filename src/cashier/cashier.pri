HEADERS += \
    $$PWD/viewmodel/models/CartItemModel.h \
    $$PWD/viewmodel/models/PaymentRecordModel.h \
    $$PWD/viewmodel/CartVM.h \
    $$PWD/viewmodel/PaymentVM.h \
    $$PWD/viewmodel/ShiftVM.h \
    $$PWD/viewmodel/CashierVM.h \
    $$PWD/repository/OrderRepo.h \
    $$PWD/service/OrderService.h \
    $$PWD/service/PaymentService.h \
    $$PWD/service/ShiftService.h

SOURCES += \
    $$PWD/viewmodel/models/CartItemModel.cpp \
    $$PWD/viewmodel/models/PaymentRecordModel.cpp \
    $$PWD/viewmodel/CartVM.cpp \
    $$PWD/viewmodel/PaymentVM.cpp \
    $$PWD/viewmodel/ShiftVM.cpp \
    $$PWD/viewmodel/CashierVM.cpp \
    $$PWD/repository/OrderRepo.cpp \
    $$PWD/service/OrderService.cpp \
    $$PWD/service/PaymentService.cpp \
    $$PWD/service/ShiftService.cpp

DISTFILES += \
    $$PWD/qml/CashierPage.qml \
    $$PWD/qml/styles/AppColors.qml \
    $$PWD/qml/styles/AppFonts.qml \
    $$PWD/qml/styles/AppTheme.qml \
    $$PWD/qml/components/TopStatusBar.qml \
    $$PWD/qml/components/CategoryTabBar.qml \
    $$PWD/qml/components/ProductCard.qml \
    $$PWD/qml/components/ProductGrid.qml \
    $$PWD/qml/components/NumberPad.qml \
    $$PWD/qml/components/BarcodeInput.qml \
    $$PWD/qml/components/CartItemDelegate.qml \
    $$PWD/qml/components/CartPanel.qml \
    $$PWD/qml/components/MemberInfo.qml \
    $$PWD/qml/components/ActionButtonBar.qml \
    $$PWD/qml/dialogs/PaymentDialog.qml \
    $$PWD/qml/dialogs/MemberSearchDialog.qml \
    $$PWD/qml/dialogs/ProductSearchDialog.qml \
    $$PWD/qml/dialogs/SuspendOrderDialog.qml \
    $$PWD/qml/dialogs/RefundDialog.qml \
    $$PWD/qml/dialogs/ShiftDialog.qml
