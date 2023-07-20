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
        width: parent.width
        height: parent.height - controlButtonItem.height - units.gu(8) - parent.height/6
        source: cli.connected ?  "../../graphics/vpn_on.png" : "../../graphics/vpn_off.png"
        fillMode: Image.PreserveAspectFit
        anchors.centerIn: parent
    }

    Item{
        id: controlButtonItem
        width: parent.width
        height: connectButton.height*2 + units.gu(12)

        anchors{
            bottom: parent.bottom
        }

        BigButton{
            id: connectButton
            text: cli.connected ? i18n.tr("Disconect") : i18n.tr("Connect")
            border.color: "#ffffff"

            width: parent.width - units.gu(6)
            anchors{
                top: controlButtonItem.top
                topMargin: units.gu(4)
                horizontalCenter: parent.horizontalCenter
            }

            onClicked: {
                if(cli.connected) {
                    cli.disconnecting("SUDO PASS")
                } else {
                    cli.makeConnection("SUDO PASS")
                }
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

    Connections{
        target: cli
        onConnectedChanged: {
            if(cli.connected) {
                connectButton.text = i18n.tr("Disconect")
            } else {
                connectButton.text = i18n.tr("Connect")
            }
        }
    }
}
