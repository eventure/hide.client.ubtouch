import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3

import "../components"

Item {
    id: connectionPage
    anchors.fill: parent

    property string sudoPass

    Rectangle{
        anchors.fill: parent
        color: "#888888"
        z: 2
        visible: loading.running

        ActivityIndicator {
            id: loading
            anchors.centerIn: parent
            running: !cli.isReady
        }

        Text{
            id: setupText
            text: i18n.tr("Prepare to connection...")
            color: "#fff"
            font.pixelSize: units.gu(2)

            anchors{
                top: loading.bottom
                topMargin: units.gu(1)
                horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Rectangle{
        id: connectioArea
        width: parent.width
        height: parent.height - bottomLine.height
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
                source: cli.connected ?  "../../graphics/vpn_on.png" : "../../graphics/vpn_off.png"
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
                if(sudoPass.length == 0) {
                    onTriggered: PopupUtils.open(Qt.resolvedUrl("../dialogs/SudoPassDialog.qml"), connectionPage)
                } else {
                    if(cli.connected) {
                        cli.disconnecting(sudoPass)
                    } else {
                        cli.makeConnection(sudoPass)
                    }
                }
            }
        }
    }

    Rectangle{
        id: bottomLine
        width: parent.width
        height: units.gu(10)

        anchors{
            bottom: parent.bottom
        }

        Row{
            id: bottomLineRow
            anchors.fill: parent
            Repeater{
                id: bottomLineRepeater
                model: ListModel{
                    id: bottomLineModel
                    property int currentIndex: 0
                    ListElement{
                        title: qsTr("VPN")
                    }
                    ListElement{
                        title: qsTr("Location")
                    }
                    ListElement{
                        title: qsTr("Settings")
                    }
                }
                delegate: DownButton{
                    text: title
                    active: bottomLineModel.currentIndex == index
                }
            }
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
