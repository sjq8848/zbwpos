import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    height: 48
    color: "#1976D2"

    property string storeName: "门店"
    property string cashierName: "收银员"
    property string shiftInfo: ""
    property bool isOnline: true

    signal settingsClicked()

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 24

        Label {
            text: root.storeName
            color: "white"
            font.bold: true
            font.pixelSize: 16
        }

        Label {
            text: "收银员: " + root.cashierName
            color: "white"
            opacity: 0.9
        }

        Label {
            text: root.shiftInfo
            color: "white"
            opacity: 0.9
            visible: root.shiftInfo !== ""
        }

        Item { Layout.fillWidth: true }

        Label {
            text: root.isOnline ? "在线" : "离线"
            color: root.isOnline ? "#A5D6A7" : "#FFCC80"
        }

        Button {
            text: "设置"
            flat: true
            onClicked: root.settingsClicked()
        }
    }
}
