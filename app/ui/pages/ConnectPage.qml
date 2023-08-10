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

    Loader{
        id: mainAreaLoader
        width: parent.width
        height: parent.height - bottomLine.height

        source: "../components/ConnectionPage/vpn.qml"
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
                        page: "../components/ConnectionPage/vpn.qml"
                    }
                    ListElement{
                        title: qsTr("Location")
                        page: "../components/ConnectionPage/location.qml"
                    }
                    ListElement{
                        title: qsTr("Settings")
                        page: "../components/ConnectionPage/settings.qml"
                    }
                }
                delegate: DownButton{
                    text: title
                    active: bottomLineModel.currentIndex == index
                    onClicked: {
                        if(bottomLineModel.currentIndex != index) {
                            bottomLineModel.currentIndex = index
                            mainAreaLoader.source = page
                        }
                    }
                }
            }
        }
    }
}
