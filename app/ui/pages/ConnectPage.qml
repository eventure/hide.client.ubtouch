import QtQuick 2.4
import Lomiri.Components 1.3

import "../components"

Item {
    id: connectionPage
    anchors.fill: parent
    Rectangle{
        anchors.fill: parent
        color: "#2AA9E0"
    }

    Image{
        id: statusImage
        width: parent.width/2
        height: width
        source: mApplication.connected ?  "../graphics/vpn_on.png" : "../graphics/vpn_off.png"
        fillMode: Image.PreserveAspectFit
        anchors.centerIn: parent
    }

    BigButton{
        id: connectButton
        text: mApplication.connected ? i18n.tr("Disconect") : i18n.tr("Connect")
        border.color: "#ffffff"

        width: parent.width - units.gu(6)
        anchors{
            top: statusImage.bottom
            topMargin: units.gu(4)
            horizontalCenter: parent.horizontalCenter
        }
    }

    BigButton{
        id: logoutButton
        text: i18n.tr("Logout")
        border.color: "#ffffff"

        width: parent.width - units.gu(6)
        anchors{
            top: connectButton.bottom
            topMargin: units.gu(4)
            horizontalCenter: parent.horizontalCenter
        }

        onClicked: mApplication.logout();
    }
}
