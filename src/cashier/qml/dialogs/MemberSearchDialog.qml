import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    modal: true
    title: "会员查询"
    standardButtons: Dialog.Close
    width: 400

    property var memberVM: null
    signal memberSelected(var member)

    ColumnLayout {
        spacing: 16
        width: parent.width

        RowLayout {
            Layout.fillWidth: true

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: "输入手机号或会员卡号"
                onAccepted: {
                    if (memberVM) memberVM.search(text)
                }
            }

            Button {
                text: "查询"
                onClicked: {
                    if (memberVM) memberVM.search(searchField.text)
                }
            }
        }

        ListView {
            id: memberList
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            clip: true
            model: memberVM ? memberVM.searchResults : null

            delegate: ItemDelegate {
                width: memberList.width
                text: modelData.name + " (" + modelData.phone + ")"
                onClicked: {
                    root.memberSelected(modelData)
                    root.close()
                }
            }
        }
    }
}
