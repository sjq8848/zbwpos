import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#FAFAFA"

    property var cart: null
    signal checkoutClicked()
    signal suspendClicked()
    signal clearClicked()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        ListView {
            id: cartList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: cart ? cart.items : null
            delegate: CartItemDelegate {
                width: cartList.width
                itemName: modelData.name || ""
                itemPrice: modelData.price || 0
                itemQuantity: modelData.quantity || 1
                itemSubtotal: modelData.subtotal || 0
                onQuantityChanged: function(qty) {
                    if (cart) cart.updateQuantity(index, qty)
                }
                onRemoveClicked: {
                    if (cart) cart.removeItem(index)
                }
            }
        }

        // Totals
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4

            RowLayout {
                Layout.fillWidth: true
                Label { text: "商品数: " + (cart ? cart.itemCount : 0) }
                Item { Layout.fillWidth: true }
                Label { text: "小计: ¥" + (cart ? cart.subtotal.toFixed(2) : "0.00") }
            }

            RowLayout {
                Layout.fillWidth: true
                visible: cart && cart.discount > 0
                Label { text: "优惠"; color: "#E91E63" }
                Item { Layout.fillWidth: true }
                Label { text: "-¥" + (cart ? cart.discount.toFixed(2) : "0.00"); color: "#E91E63" }
            }

            Rectangle { Layout.fillWidth: true; height: 1; color: "#E0E0E0" }

            RowLayout {
                Layout.fillWidth: true
                Label { text: "应收:"; font.bold: true }
                Item { Layout.fillWidth: true }
                Label {
                    text: "¥" + (cart ? cart.total.toFixed(2) : "0.00")
                    font.bold: true
                    font.pixelSize: 20
                    color: "#E91E63"
                }
            }
        }

        // Action buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                text: "挂单"
                Layout.fillWidth: true
                onClicked: root.suspendClicked()
            }
            Button {
                text: "清空"
                Layout.fillWidth: true
                onClicked: root.clearClicked()
            }
            Button {
                text: "结算"
                Layout.fillWidth: true
                highlighted: true
                onClicked: root.checkoutClicked()
            }
        }
    }
}
