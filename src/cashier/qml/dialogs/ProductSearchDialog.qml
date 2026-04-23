import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    modal: true
    title: "商品查询"
    standardButtons: Dialog.Close
    width: 500

    property var productVM: null
    signal productSelected(var product)

    ColumnLayout {
        spacing: 16
        width: parent.width

        RowLayout {
            Layout.fillWidth: true

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: "输入商品名称或条码"
                onAccepted: {
                    if (productVM) productVM.search(text)
                }
            }

            Button {
                text: "查询"
                onClicked: {
                    if (productVM) productVM.search(searchField.text)
                }
            }
        }

        ListView {
            id: productList
            Layout.fillWidth: true
            Layout.preferredHeight: 300
            clip: true
            model: productVM ? productVM.searchResults : null

            delegate: ItemDelegate {
                width: productList.width
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    Label {
                        text: modelData.name
                        Layout.fillWidth: true
                    }
                    Label {
                        text: modelData.barcode || ""
                        color: "#757575"
                    }
                    Label {
                        text: "¥" + (modelData.price ? modelData.price.toFixed(2) : "0.00")
                        color: "#E91E63"
                        font.bold: true
                    }
                }
                onClicked: {
                    root.productSelected(modelData)
                    root.close()
                }
            }
        }
    }
}
