import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    height: visible ? 60 : 0
    color: "#E8F5E9"
    radius: 4
    visible: memberName !== ""

    property string memberName: ""
    property string memberPhone: ""
    property string memberLevel: ""
    property real memberBalance: 0
    property real memberPoints: 0

    signal clearMember()

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 16

        Label {
            text: "会员: " + root.memberName
            font.bold: true
        }

        Label {
            text: root.memberPhone
            color: "#757575"
        }

        Label {
            text: root.memberLevel
            color: "#FF9800"
            visible: root.memberLevel !== ""
        }

        Label {
            text: "余额: ¥" + root.memberBalance.toFixed(2)
            color: "#4CAF50"
        }

        Label {
            text: "积分: " + root.memberPoints
            color: "#2196F3"
        }

        Item { Layout.fillWidth: true }

        Button {
            text: "取消"
            flat: true
            onClicked: root.clearMember()
        }
    }
}
