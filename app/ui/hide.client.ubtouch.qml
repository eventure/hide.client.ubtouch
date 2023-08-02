import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3

MainView {
    id: mainView
    objectName: "vpnMainView"
    applicationName: "hide.client.ubtouch"

    width: parent.width
    height: parent.height

    backgroundColor: "white"

    PageStack {
        id: mainStack
        anchors.fill: parent

        Component.onCompleted: {
            if(!cli.cliAvailable) {
                push(Qt.resolvedUrl("pages/NoToolsPage.qml"), {})
            } else {
                if(cli.isLogined) {
                    push(Qt.resolvedUrl("pages/ConnectPage.qml"))
                } else {
                    push(Qt.resolvedUrl("pages/LoginPage.qml"), {})
                }
            }
        }
    }

    Connections{
        target: mApplication
        onIsLoginedChanged: {
            timoutTimer.stop();
            if(mApplication.isLogined) {
                mainStack.push(Qt.resolvedUrl("pages/ConnectPage.qml"))
            } else {
                mainStack.push(Qt.resolvedUrl("pages/LoginPage.qml"), {})
            }
        }
        onLoginFailed: {
            timoutTimer.stop();
            PopupUtils.open(Qt.resolvedUrl("dialogs/LoginFailedDialog.qml"), mainView)
        }
    }

    Timer {
        id: timoutTimer
        interval: 2500;
        onTriggered: PopupUtils.open(Qt.resolvedUrl("dialogs/TimeOutDialog.qml"), mainView)
    }
}
