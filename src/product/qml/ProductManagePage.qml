import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: root
    title: "商品管理"

    property var productVM: null  // set from C++

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16

        // Search bar
        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: searchField
                placeholderText: "搜索商品名称或条码..."
                Layout.fillWidth: true
                onAccepted: {
                    productVM.keyword = text
                    productVM.search()
                }
            }
            Button {
                text: "搜索"
                onClicked: {
                    productVM.keyword = searchField.text
                    productVM.search()
                }
            }
        }

        // Product list
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: productVM.products
            delegate: ItemDelegate {
                width: ListView.view.width
                height: 60
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    Label { text: model.name; Layout.fillWidth: true }
                    Label { text: "¥" + model.price.toFixed(2); color: "#e91e63" }
                    Label { text: "库存: " + model.stock; color: "#666" }
                }
            }
            ScrollBar.vertical: ScrollBar {}
        }
    }
}
