import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    visible: true
    width: 1280
    height: 800
    title: "ZBW POS"

    property var app: App  // set from C++ via context property

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: app && app.isLoggedIn ? "qrc:/CashierPage.qml" : "qrc:/LoginPage.qml"
    }

    Connections {
        target: app
        function onLoginStateChanged() {
            if (app.isLoggedIn) {
                stackView.clear()
                stackView.push("qrc:/CashierPage.qml")
            } else {
                stackView.clear()
                stackView.push("qrc:/LoginPage.qml")
            }
        }
    }
}
