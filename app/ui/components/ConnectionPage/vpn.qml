import QtQuick 2.4
import Lomiri.Components 1.3

import "../"

Rectangle{
    id: vpnArea
    width: parent.width
    height: parent.height
    color: "#2AA9E0"

    Item{
        id: statusItem
        implicitHeight: units.gu(25)
        width: parent.width
        anchors.bottom: parent.verticalCenter

        Image{
            id: statusImage
            width: height
            height: units.gu(16)
            source: cli.connected ?  "../../../graphics/vpn_on.png" : "../../../graphics/vpn_off.png"
            fillMode: Image.PreserveAspectFit
            anchors{
                top: parent.top
                topMargin: units.gu(2)
                horizontalCenter: parent.horizontalCenter
            }
        }

        Text{
            id: locatioText
            text: i18n.tr("Best Location")
            color: "#fff"
            font.pixelSize: units.gu(2)
            anchors{
                bottom: parent.bottom
                bottomMargin: units.gu(1)
                horizontalCenter: parent.horizontalCenter
            }
        }
    }

    BigButton{
        id: connectButton
        text: cli.connected ? i18n.tr("Enable VPN") : i18n.tr("Enable VPN")
        border.color: "#ffffff"
        height: units.gu(8)
        width: parent.width - units.gu(6)
        anchors{
            top: statusItem.bottom
            topMargin: units.gu(3)
            horizontalCenter: parent.horizontalCenter
        }

        onClicked: {
            cli.connected = !cli.connected
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
            cli.connected = !cli.connected
        }
    }
}
