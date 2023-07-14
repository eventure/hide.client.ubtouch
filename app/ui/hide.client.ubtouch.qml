import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3

MainView {
    objectName: "vpnMainView"
    id: mainView

    applicationName: "hide.client.ubtouch"

    backgroundColor: "white"
    theme.name: "Lomiri.Components.Themes.Suru"

    PageStack {
        id: mainStack
        Component.onCompleted: {
            if(!haveTools) {
                push(Qt.resolvedUrl("pages/NoToolsPage.qml"), {})
            } else {
                if(mApplication.isLogined) {
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
            if(mApplication.isLogined) {
                mainStack.push(Qt.resolvedUrl("pages/ConnectPage.qml"))
            } else {
                mainStack.push(Qt.resolvedUrl("pages/LoginPage.qml"), {})
            }
        }
    }
}
