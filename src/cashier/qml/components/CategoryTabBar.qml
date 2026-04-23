import QtQuick 2.15
import QtQuick.Controls 2.15

Row {
    id: root
    spacing: 8

    property var categories: []
    property int currentIndex: 0

    signal categorySelected(int categoryId)

    Repeater {
        model: root.categories
        delegate: Button {
            text: modelData.name
            checked: root.currentIndex === index
            onClicked: {
                root.currentIndex = index
                root.categorySelected(modelData.id || index)
            }
        }
    }
}
