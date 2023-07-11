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
            if(mApplication.isLogined) {
                console.log("LOGINED")
            } else {
                push(Qt.resolvedUrl("pages/LoginPage.qml"), {})
            }
        }
    }
}
