import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: root
    title: "ZBW POS - 登录"

    property var app: App

    background: Rectangle {
        color: "#F5F5F5"
    }

    ColumnLayout {
        anchors.centerIn: parent
        width: 400
        spacing: 16

        Label {
            text: "ZBW POS"
            font.pixelSize: 32
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: "智能收银系统"
            font.pixelSize: 14
            color: "#666666"
            Layout.alignment: Qt.AlignHCenter
        }

        Item { height: 16 }

        TextField {
            id: serverField
            placeholderText: "服务器地址"
            Layout.fillWidth: true
            text: Settings ? Settings.serverUrl : "https://api.example.com"
            selectByMouse: true
        }

        TextField {
            id: usernameField
            placeholderText: "用户名"
            Layout.fillWidth: true
            selectByMouse: true
        }

        TextField {
            id: passwordField
            placeholderText: "密码"
            Layout.fillWidth: true
            echoMode: TextInput.Password
            selectByMouse: true
        }

        Item { height: 8 }

        Button {
            id: loginButton
            text: "登录"
            Layout.fillWidth: true
            highlighted: true
            enabled: !loginButton._isProcessing

            property bool _isProcessing: false

            background: Rectangle {
                color: loginButton.enabled ? (loginButton.pressed ? "#1976D2" : "#2196F3") : "#BDBDBD"
                radius: 4
            }

            contentItem: Text {
                text: loginButton._isProcessing ? "登录中..." : loginButton.text
                font.pixelSize: 16
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
                if (app) {
                    loginButton._isProcessing = true;
                    var success = app.login(serverField.text, usernameField.text, passwordField.text);
                    loginButton._isProcessing = false;

                    if (!success) {
                        errorLabel.text = "登录失败，请检查用户名和密码";
                        errorLabel.visible = true;
                    }
                }
            }
        }

        Label {
            id: errorLabel
            color: "#F44336"
            visible: text !== ""
            Layout.alignment: Qt.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        Item { height: 8 }

        Label {
            text: "Powered by ZBW"
            font.pixelSize: 11
            color: "#999999"
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
