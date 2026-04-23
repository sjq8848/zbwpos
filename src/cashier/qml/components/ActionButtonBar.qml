import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    height: 56
    color: "#F5F5F5"

    signal memberQuery()
    signal productQuery()
    signal refund()
    signal shift()

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        Button {
            text: "会员查询"
            onClicked: root.memberQuery()
        }

        Button {
            text: "商品查询"
            onClicked: root.productQuery()
        }

        Button {
            text: "退货退款"
            onClicked: root.refund()
        }

        Button {
            text: "交班"
            onClicked: root.shift()
        }

        Item { Layout.fillWidth: true }
    }
}
