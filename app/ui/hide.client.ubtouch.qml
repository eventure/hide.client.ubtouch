import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3

import hide.me 1.0

import "authentication"

MainView {
    id: mainView
    objectName: "vpnMainView"
    applicationName: "hide.client.ubtouch"

    property bool fullAccessGranted: false
    property string errorTitle;
    property string errorMessage;

    width: parent.width
    height: parent.height

    backgroundColor: "white"

    PageStack {
        id: mainStack
        anchors.fill: parent

        Component.onCompleted: initPage();
    }

    ServerSelectionModel{
        id: serverSelectionModel
//        onServerActivated: console.log("ACTIVATED >>>> " + server["displayName"] + server["serverId"])
    }

    ServiceManager{
        id: serviceManager
    }

    CliToolConnector{
        id: cli
        onTokenChanged: serviceManager.setAccessToken(cli.token)
    }

    AuthenticationHandler {
        id: authentication
        serviceName: "hideme"
        onAuthenticationSucceeded: fullAccessGranted = true
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

        onLoginSuccess: {
            timoutTimer.stop();
            mainStack.push(Qt.resolvedUrl("pages/ConnectPage.qml"))
        }

        onError: {
            mainView.errorTitle = title
            mainView.errorMessage = message
            PopupUtils.open(Qt.resolvedUrl("dialogs/ErrorDialog.qml"), mainView)
        }
    }

    Timer {
        id: timoutTimer
        interval: 5000;
        onTriggered: PopupUtils.open(Qt.resolvedUrl("dialogs/TimeOutDialog.qml"), mainView)
    }

    function initPage() {
        if(!serviceManager.cliAvailable) {
            mainStack.push(Qt.resolvedUrl("pages/NoToolsPage.qml"), {})
        } else {
            if(cli.isLogined) {
                cli.getTokenRequest();
                mainStack.push(Qt.resolvedUrl("pages/ConnectPage.qml"), {})
            } else {
                mainStack.push(Qt.resolvedUrl("pages/LoginPage.qml"), {})
            }
        }
    }
}
