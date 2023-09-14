import QtQuick 2.4
import Lomiri.Components 1.3
import hide.me 1.0

import "../"

Rectangle{
    id: vpnArea
    width: parent.width
    height: parent.height
    color: "#2AA9E0"

    function calculateStatusProp() {
        switch(serviceManager.currentStatus) {
        case ServiceManager.UNKNOW:
        case ServiceManager.NOT_INSTALLED:
            statusImage.source = "../../../graphics/vpn_warn.png"
            connectButton.text = qsTr("Setup service first")
            return;
        case ServiceManager.NOT_STARTED:
            statusImage.source = "../../../graphics/vpn_warn.png"
            connectButton.text = qsTr("Service not started")
            return;
        case ServiceManager.STARTED:
        case ServiceManager.DISCONNECTED:
            statusImage.source ="../../../graphics/vpn_off.png";
            connectButton.text = qsTr("Connect")
            return;
        case ServiceManager.DISCONNECTING:
            statusImage.source = "../../../graphics/vpn_inprogress.png";
            connectButton.text = qsTr("Disconnecting...")
            return;
        case ServiceManager.CONNECTING:
            statusImage.source = "../../../graphics/vpn_inprogress.png";
            connectButton.text = qsTr("Connecting...")
            return;
        case ServiceManager.CONNECTED:
            statusImage.source = "../../../graphics/vpn_on.png";
            connectButton.text = qsTr("Disconnect")
            return;
        }
    }

    Component.onCompleted: {
        calculateStatusProp();
        var location = serverSelectionModel.get(cli.hostName)["displayName"]
        if(location != "") {
            locationText.text = location
        }
    }

    Connections{
        target: serverSelectionModel
        onRowCountChanged: locationText.text = serverSelectionModel.get(cli.hostName)["displayName"]
    }

    Connections{
        target: cli
        onHostNameChanged: locationText.text = serverSelectionModel.get(cli.hostName)["displayName"]
    }

    Item{
        id: statusItem
        implicitHeight: units.gu(25)
        width: parent.width
        anchors.bottom: parent.verticalCenter

        Image{
            id: statusImage
            width: height
            height: units.gu(16)
            fillMode: Image.PreserveAspectFit
            anchors{
                top: parent.top
                topMargin: units.gu(2)
                horizontalCenter: parent.horizontalCenter
            }
        }

        Text{
            id: locationText
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
        border.color: "#ffffff"
        height: units.gu(8)
        width: parent.width - units.gu(6)
        anchors{
            top: statusItem.bottom
            topMargin: units.gu(3)
            horizontalCenter: parent.horizontalCenter
        }

        onClicked: {
            if(serviceManager.currentStatus == ServiceManager.NOT_INSTALLED
                    || serviceManager.currentStatus == ServiceManager.UNKNOW
                    || serviceManager.currentStatus == ServiceManager.NOT_STARTED) {
                // go to settings page
                bottomLineModel.currentIndex = 2
            } else if(serviceManager.currentStatus == ServiceManager.STARTED ) {
                cli.makeConnection()
            } else if(serviceManager.currentStatus == ServiceManager.CONNECTED) {
                cli.makeDisconnection()
            }
        }
    }

    Connections{
        target: serviceManager
        onCurrentStatusChanged: calculateStatusProp()
    }
}
