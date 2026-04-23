import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "components"
import "dialogs"
import "styles"

Page {
    id: root

    property var cashierVM: null

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TopStatusBar {
            id: statusBar
            Layout.fillWidth: true
            storeName: "测试门店"
            cashierName: "收银员"
            isOnline: true
        }

        MemberInfo {
            id: memberInfo
            Layout.fillWidth: true
            memberName: cashierVM && cashierVM.currentMember ? cashierVM.currentMember.name : ""
            memberPhone: cashierVM && cashierVM.currentMember ? cashierVM.currentMember.phone : ""
            memberLevel: cashierVM && cashierVM.currentMember ? cashierVM.currentMember.level : ""
            memberBalance: cashierVM && cashierVM.currentMember ? cashierVM.currentMember.balance : 0
            memberPoints: cashierVM && cashierVM.currentMember ? cashierVM.currentMember.points : 0
            onClearMember: {
                if (cashierVM) cashierVM.clearMember()
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 1

            // Left: Product area
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: 8

                CategoryTabBar {
                    id: categoryBar
                    Layout.fillWidth: true
                    categories: cashierVM ? cashierVM.categories : []
                    onCategorySelected: function(id) {
                        if (cashierVM) cashierVM.selectCategory(id)
                    }
                }

                ProductGrid {
                    id: productGrid
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    products: cashierVM ? cashierVM.products : []
                    onProductSelected: function(product) {
                        if (cashierVM) cashierVM.addToCart(product)
                    }
                }

                BarcodeInput {
                    id: barcodeInput
                    Layout.fillWidth: true
                    onBarcodeEntered: function(barcode) {
                        if (cashierVM) cashierVM.scanBarcode(barcode)
                    }
                }
            }

            Rectangle {
                Layout.fillHeight: true
                width: 1
                color: "#E0E0E0"
            }

            // Right: Cart
            CartPanel {
                id: cartPanel
                Layout.preferredWidth: 350
                Layout.fillHeight: true
                cart: cashierVM ? cashierVM.cart : null
                onCheckoutClicked: {
                    if (cashierVM) cashierVM.startPayment()
                }
                onSuspendClicked: {
                    if (cashierVM) cashierVM.suspendOrder()
                }
                onClearClicked: {
                    if (cashierVM) cashierVM.clearAll()
                }
            }
        }

        ActionButtonBar {
            id: actionBar
            Layout.fillWidth: true
            onMemberQuery: memberSearchDialog.open()
            onProductQuery: productSearchDialog.open()
            onRefund: refundDialog.open()
            onShift: shiftDialog.open()
        }
    }

    PaymentDialog {
        id: paymentDialog
        paymentVM: cashierVM ? cashierVM.payment : null
        onAccepted: {
            if (cashierVM) cashierVM.clearAll()
        }
    }

    MemberSearchDialog {
        id: memberSearchDialog
        onMemberSelected: function(member) {
            if (cashierVM) cashierVM.setMember(member)
        }
    }

    ProductSearchDialog {
        id: productSearchDialog
        onProductSelected: function(product) {
            if (cashierVM) cashierVM.addToCart(product)
        }
    }

    SuspendOrderDialog {
        id: suspendOrderDialog
        cashierVM: root.cashierVM
        onOrderResumed: function(order) {
            if (cashierVM) cashierVM.resumeOrder(order)
        }
    }

    RefundDialog {
        id: refundDialog
    }

    ShiftDialog {
        id: shiftDialog
        onShiftCompleted: {
            console.log("Shift completed")
        }
    }

    Connections {
        target: cashierVM
        function onShowPaymentDialog() {
            paymentDialog.open()
        }
        function onOrderCompleted(orderNo) {
            console.log("Order completed: " + orderNo)
        }
    }
}
