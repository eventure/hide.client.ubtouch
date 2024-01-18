import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3

import hide.me 1.0

import "authentication"

MainView {
    id: mainView
    objectName: "hidemeVPN"
    applicationName: "hide.client.ubtouch"

    property bool fullAccessGranted: false
    property string errorTitle;
    property string errorMessage;

    width: units.gu(45)
    height: units.gu(75)

    backgroundColor: "white"

    PageStack {
        id: mainStack
        Component.onCompleted: initPage();
    }

    ServerSelectionModel{
        id: serverSelectionModel
    }

    ServiceManager{
        id: serviceManager
        onCurrentStatusChanged: {
            if(serviceManager.currentStatus == ServiceManager.STARTED) {
                cli.initServiceSetup()
            }

            if(serviceManager.currentStatus == ServiceManager.CONNECTED ||
                serviceManager.currentStatus == ServiceManager.DISCONNECTED) {
                cli.makeRoute()
            }
        }
    }

    CliToolConnector{
        id: cli
        onTokenChanged: serviceManager.setAccessToken(cli.token)
        onError: cli.makeDisconnection()
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
                mainStack.clear()
                cli.makeDisconnection()
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
            PopupUtils.open(Qt.resolvedUrl("dialogs/ErrorDialog.qml"), mainView, {
                                title: mainView.errorTitle,
                                message: mainView.errorMessage
                            })
        }
        onSetupServiceFail: {
            initServiceTimer.start()
        }
    }

    Timer {
        id: timoutTimer
        interval: 5000;
        onTriggered: PopupUtils.open(Qt.resolvedUrl("dialogs/TimeOutDialog.qml"), mainView)
    }

    Timer{
        id: initServiceTimer
        interval: 1000
        onTriggered: cli.initServiceSetup()
    }

    function initPage() {
        if(!serviceManager.cliAvailable) {
            mainStack.push(Qt.resolvedUrl("pages/NoToolsPage.qml"), {})
        } else {
            if(cli.isLogined) {
                mainStack.push(Qt.resolvedUrl("pages/ConnectPage.qml"), {})
            } else {
                mainStack.push(Qt.resolvedUrl("pages/LoginPage.qml"), {})
            }
        }
    }
}
