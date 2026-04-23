import QtQuick 2.15
import QtQuick.Controls 2.15

GridView {
    id: root
    cellWidth: 130
    cellHeight: 150
    clip: true

    property var products: []

    signal productSelected(var product)

    model: root.products

    delegate: ProductCard {
        productName: modelData.name
        productPrice: modelData.price ? modelData.price.toFixed(2) : "0.00"
        productImage: modelData.image || ""
        onClicked: root.productSelected(modelData)
    }

    ScrollBar.vertical: ScrollBar {}
}
