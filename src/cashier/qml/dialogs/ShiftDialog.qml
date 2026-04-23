import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    modal: true
    title: "交班"
    standardButtons: Dialog.Close
    width: 400

    property var shiftVM: null
    signal shiftCompleted()

    ColumnLayout {
        spacing: 16
        width: parent.width

        Label {
            text: "当前班次信息"
            font.bold: true
            font.pixelSize: 16
            Layout.fillWidth: true
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            rowSpacing: 8
            columnSpacing: 16

            Label { text: "上班时间:" }
            Label {
                text: shiftVM ? Qt.formatDateTime(shiftVM.startTime, "yyyy-MM-dd hh:mm") : "--"
                font.bold: true
            }

            Label { text: "订单数:" }
            Label {
                text: shiftVM ? shiftVM.orderCount.toString() : "0"
                font.bold: true
            }

            Label { text: "销售总额:" }
            Label {
                text: "¥" + (shiftVM ? shiftVM.totalSales.toFixed(2) : "0.00")
                font.bold: true
                color: "#E91E63"
            }

            Label { text: "现金:" }
            Label {
                text: "¥" + (shiftVM ? shiftVM.cashTotal.toFixed(2) : "0.00")
                font.bold: true
            }

            Label { text: "电子支付:" }
            Label {
                text: "¥" + (shiftVM ? shiftVM.electronicTotal.toFixed(2) : "0.00")
                font.bold: true
            }

            Label { text: "退款:" }
            Label {
                text: "¥" + (shiftVM ? shiftVM.refundTotal.toFixed(2) : "0.00")
                color: "#F44336"
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: "#E0E0E0" }

        Label {
            text: "现金清点:"
            Layout.fillWidth: true
        }

        TextField {
            id: actualCashField
            Layout.fillWidth: true
            placeholderText: "输入实际现金金额"
        }

        Label {
            visible: actualCashField.text.length > 0 && shiftVM
            text: {
                var actual = parseFloat(actualCashField.text) || 0
                var expected = shiftVM ? shiftVM.cashTotal : 0
                var diff = actual - expected
                return "差异: ¥" + diff.toFixed(2)
            }
            color: {
                var actual = parseFloat(actualCashField.text) || 0
                var expected = shiftVM ? shiftVM.cashTotal : 0
                return actual >= expected ? "#4CAF50" : "#F44336"
            }
            Layout.fillWidth: true
        }

        Button {
            text: "确认交班"
            highlighted: true
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 200
            onClicked: {
                if (shiftVM) {
                    shiftVM.completeShift(parseFloat(actualCashField.text) || 0)
                }
                root.shiftCompleted()
                root.close()
            }
        }
    }
}
