import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ItemDelegate {
    id: root
    height: 56

    property string itemName: ""
    property real itemPrice: 0
    property int itemQuantity: 1
    property real itemSubtotal: 0

    signal quantityChanged(int qty)
    signal removeClicked()

    contentItem: RowLayout {
        spacing: 8

        Label {
            text: root.itemName
            Layout.fillWidth: true
            elide: Text.ElideRight
            font.pixelSize: 14
        }

        SpinBox {
            value: root.itemQuantity
            from: 1
            to: 999
            implicitWidth: 100
            onValueModified: root.quantityChanged(value)
        }

        Label {
            text: "¥" + root.itemSubtotal.toFixed(2)
            color: "#E91E63"
            font.bold: true
            font.pixelSize: 14
            Layout.preferredWidth: 70
            horizontalAlignment: Text.AlignRight
        }

        Button {
            text: "×"
            flat: true
            font.pixelSize: 16
            onClicked: root.removeClicked()
        }
    }
}
