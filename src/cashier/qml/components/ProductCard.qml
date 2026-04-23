import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    width: 120
    height: 140
    radius: 8
    color: mouse.containsMouse ? "#E3F2FD" : "#FFFFFF"
    border.color: "#E0E0E0"

    property string productName: "商品"
    property string productPrice: "0.00"
    property string productImage: ""

    signal clicked()

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }

    Column {
        anchors.centerIn: parent
        spacing: 8

        Rectangle {
            width: 80
            height: 80
            color: "#F5F5F5"
            radius: 4
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            text: root.productName
            anchors.horizontalCenter: parent.horizontalCenter
            elide: Text.ElideRight
            width: 100
            horizontalAlignment: Text.AlignHCenter
        }

        Label {
            text: "¥" + root.productPrice
            color: "#E91E63"
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
