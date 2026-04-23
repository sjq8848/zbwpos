import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    modal: true
    title: "挂单列表"
    standardButtons: Dialog.Close
    width: 400

    property var cashierVM: null
    signal orderResumed(var order)

    ColumnLayout {
        spacing: 16
        width: parent.width

        Label {
            text: "选择要恢复的挂单:"
            Layout.fillWidth: true
        }

        ListView {
            id: suspendList
            Layout.fillWidth: true
            Layout.preferredHeight: 300
            clip: true
            model: cashierVM ? cashierVM.suspendedOrders : null

            delegate: ItemDelegate {
                width: suspendList.width
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    Label {
                        text: modelData.orderNo || ("挂单#" + index)
                    }
                    Label {
                        text: modelData.itemCount + "件商品"
                    }
                    Label {
                        text: "¥" + modelData.total.toFixed(2)
                        color: "#E91E63"
                    }
                    Label {
                        text: Qt.formatDateTime(modelData.suspendTime, "hh:mm:ss")
                        color: "#757575"
                    }
                }
                onClicked: {
                    root.orderResumed(modelData)
                    root.close()
                }
            }
        }
    }
}
