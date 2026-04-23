import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    modal: true
    title: "退货退款"
    standardButtons: Dialog.Close
    width: 400

    property var refundVM: null
    signal refundCompleted(var refundInfo)

    ColumnLayout {
        spacing: 16
        width: parent.width

        Label {
            text: "输入原订单号:"
            Layout.fillWidth: true
        }

        TextField {
            id: orderNoField
            Layout.fillWidth: true
            placeholderText: "订单号"
        }

        Button {
            text: "查询订单"
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                if (refundVM) refundVM.queryOrder(orderNoField.text)
            }
        }

        ListView {
            id: refundItemList
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            clip: true
            model: refundVM ? refundVM.refundItems : null
            visible: model && model.count > 0

            delegate: CheckDelegate {
                text: modelData.name + " x" + modelData.quantity + " ¥" + modelData.subtotal.toFixed(2)
                checked: modelData.selected
                onCheckedChanged: {
                    if (refundVM) refundVM.toggleRefundItem(index, checked)
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            visible: refundVM && refundVM.refundTotal > 0

            Label { text: "退款金额:" }
            Item { Layout.fillWidth: true }
            Label {
                text: "¥" + (refundVM ? refundVM.refundTotal.toFixed(2) : "0.00")
                color: "#E91E63"
                font.bold: true
                font.pixelSize: 18
            }
        }

        Button {
            text: "确认退款"
            highlighted: true
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 200
            enabled: refundVM && refundVM.refundTotal > 0
            onClicked: {
                if (refundVM) {
                    var info = refundVM.confirmRefund()
                    root.refundCompleted(info)
                }
                root.close()
            }
        }
    }
}
