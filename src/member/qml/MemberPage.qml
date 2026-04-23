import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: root
    title: "会员管理"

    property var memberVM: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16

        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: searchField
                placeholderText: "搜索会员卡号/手机号/姓名..."
                Layout.fillWidth: true
                onAccepted: {
                    memberVM.keyword = text
                    memberVM.search()
                }
            }
            Button {
                text: "搜索"
                onClicked: {
                    memberVM.keyword = searchField.text
                    memberVM.search()
                }
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: memberVM.members
            delegate: ItemDelegate {
                width: ListView.view.width
                height: 70
                ColumnLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    RowLayout {
                        Label { text: model.name; font.bold: true }
                        Label { text: model.cardNo; color: "#666" }
                        Label { text: model.phone; color: "#666" }
                    }
                    RowLayout {
                        Label { text: "积分: " + model.points }
                        Label { text: "余额: ¥" + model.balance.toFixed(2); color: "#e91e63" }
                    }
                }
            }
            ScrollBar.vertical: ScrollBar {}
        }
    }
}
