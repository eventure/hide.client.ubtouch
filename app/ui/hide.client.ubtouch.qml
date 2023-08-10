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

        Component.onCompleted: initPage();
    }

    Connections{
        target: cli
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

        onServerStaertedChanged: {
            if(!cli.serverStaerted) {
                mainStack.clear();
                mainStack.push(Qt.resolvedUrl("pages/StartServicePage.qml"), {})
            } else {
                initPage()
            }
        }

        onIsReadyChanged: {
            if(cli.isReady) {
                checkStatus.start();
                initPage()
            } else {
                mainStack.clear();
                mainStack.push(Qt.resolvedUrl("pages/StartServicePage.qml"), {})
            }
        }
    }

    Timer {
        id: timoutTimer
        interval: 2500;
        onTriggered: PopupUtils.open(Qt.resolvedUrl("dialogs/TimeOutDialog.qml"), mainView)
    }

    function initPage() {
        if(!cli.cliAvailable) {
            mainStack.push(Qt.resolvedUrl("pages/NoToolsPage.qml"), {})
        } else {
            if(!cli.serverStaerted) {
                mainStack.push(Qt.resolvedUrl("pages/StartServicePage.qml"), {})
            } else  if(cli.isLogined) {
                mainStack.push(Qt.resolvedUrl("pages/ConnectPage.qml"), {})
            } else {
                mainStack.push(Qt.resolvedUrl("pages/LoginPage.qml"), {})
            }
        }
    }
}
