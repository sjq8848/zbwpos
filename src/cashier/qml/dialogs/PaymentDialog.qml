import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    modal: true
    title: "结算"
    standardButtons: Dialog.Cancel
    width: 400

    property var paymentVM: null

    ColumnLayout {
        spacing: 16
        width: parent.width

        Label {
            text: "应收: ¥" + (paymentVM ? paymentVM.amountDue.toFixed(2) : "0.00")
            font.pixelSize: 28
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        // Payment methods
        RowLayout {
            spacing: 8
            Layout.alignment: Qt.AlignHCenter
            Repeater {
                model: [
                    { method: "cash", name: "现金" },
                    { method: "wechat", name: "微信" },
                    { method: "alipay", name: "支付宝" },
                    { method: "card", name: "银行卡" },
                    { method: "member_balance", name: "储值" }
                ]
                delegate: Button {
                    text: name
                    checked: paymentVM && paymentVM.currentMethod === method
                    onClicked: paymentVM.setMethod(method)
                }
            }
        }

        // Quick cash buttons
        RowLayout {
            spacing: 8
            visible: paymentVM && paymentVM.currentMethod === "cash"
            Layout.alignment: Qt.AlignHCenter
            Repeater {
                model: [10, 20, 50, 100, 200]
                delegate: Button {
                    text: "¥" + modelData
                    onClicked: paymentVM.quickCash(modelData)
                }
            }
        }

        // Amount input
        TextField {
            id: amountInput
            placeholderText: "输入金额"
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 200
            onAccepted: {
                paymentVM.inputAmount(parseFloat(text))
                paymentVM.addPartialPayment()
            }
        }

        // Change display
        Label {
            visible: paymentVM && paymentVM.currentMethod === "cash"
            text: "找零: ¥" + (paymentVM ? paymentVM.change.toFixed(2) : "0.00")
            color: "#4CAF50"
            font.pixelSize: 18
            Layout.alignment: Qt.AlignHCenter
        }

        // Confirm button
        Button {
            text: "确认收款"
            enabled: paymentVM && paymentVM.canComplete
            highlighted: true
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 200
            onClicked: {
                paymentVM.complete()
                root.accept()
            }
        }
    }
}
