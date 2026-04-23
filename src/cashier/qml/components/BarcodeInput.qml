import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    height: 44
    color: "#FFFFFF"
    border.color: "#E0E0E0"
    radius: 4

    property alias barcodeText: barcodeField.text

    signal barcodeEntered(string barcode)

    RowLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 8

        Label {
            text: "条码:"
            font.pixelSize: 14
        }

        TextField {
            id: barcodeField
            Layout.fillWidth: true
            placeholderText: "扫描或输入条码"
            onAccepted: {
                if (text.length > 0) {
                    root.barcodeEntered(text)
                    text = ""
                }
            }
        }

        Button {
            text: "查询"
            onClicked: {
                if (barcodeField.text.length > 0) {
                    root.barcodeEntered(barcodeField.text)
                    barcodeField.text = ""
                }
            }
        }
    }
}
