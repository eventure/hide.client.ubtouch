import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3
import "../components"

Page {
    id: loginPage
    anchors.fill: parent

    header: PageHeader {
        title: "hide.me VPN"
    }


    Column {
        id: loginPageColumn
        width: parent.width - units.gu(8)
        spacing: units.gu(4)
        visible: !loading.running
        clip: true

        anchors {
            top: parent.top
            topMargin: units.gu(20)
            left: parent.left
            leftMargin: units.gu(4)
        }

        Item {
            id: loginGroup
            width: parent.width
            height: loginButton.height*2

            LoginPassField{
                id: usernameField
                width: parent.width
                height: loginButton.height
                cornerSide: RoundedRectangle.Direction.Up
                color: "white"
                borderWidth: 1
                borderColor: "#DDDDDD"
                placeholderText: i18n.tr("Username")
            }

            LoginPassField{
                id: passwordField
                width: parent.width
                height: loginButton.height
                cornerSide: RoundedRectangle.Direction.Down
                color: "white"
                borderWidth: 1
                borderColor: "#DDDDDD"
                placeholderText: i18n.tr("Password")
                echoMode: TextInput.Password

                anchors{
                    top: usernameField.bottom
                }
            }
        }

        BigButton{
            id: loginButton
            width: parent.width
            text: qsTr("Login")
            onClicked: {
                timoutTimer.start();
                loading.running = true
                mApplication.tryLogin(usernameField.text, passwordField.text)
            }
        }
    }

    ActivityIndicator {
        id: loading
        anchors.centerIn: parent
        running: false
        z: 1
    }

    Connections{
        target: cli
        onLoginFailed: {
            console.log("FAIL!")
            timoutTimer.stop();
            loading.running = false
            usernameField.text = ""
            passwordField.text = ""
        }
        onLoginSuccess: {
            console.log("SUCC!")
            timoutTimer.stop();
        }
    }
}
