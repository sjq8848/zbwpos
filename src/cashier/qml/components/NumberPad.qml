import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Grid {
    id: root
    columns: 3
    rowSpacing: 8
    columnSpacing: 8

    property string displayText: ""

    signal digitClicked(string digit)
    signal clearClicked()
    signal backspaceClicked()
    signal confirmClicked()

    Repeater {
        model: ["1", "2", "3", "4", "5", "6", "7", "8", "9", ".", "0", "⌫"]
        delegate: Button {
            width: 64
            height: 48
            text: modelData
            onClicked: {
                if (modelData === "⌫")
                    root.backspaceClicked()
                else
                    root.digitClicked(modelData)
            }
        }
    }

    Row {
        Layout.columnSpan: 3
        spacing: 8

        Button {
            text: "清空"
            width: 100
            onClicked: root.clearClicked()
        }
        Button {
            text: "确认"
            width: 100
            highlighted: true
            onClicked: root.confirmClicked()
        }
    }
}
