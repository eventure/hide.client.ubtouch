import QtQuick 2.4
import QtGraphicalEffects 1.12

Rectangle {
    id: serverSelectionItemLine
    width: parent.width
    height: units.gu(6)

    property bool favorite: false
    property bool available: true
    property bool primary: true
    property alias countryName: countryName.text
    property alias show10g: is10gImage.visible
    property string flag: ""
    property int serverId: -1

    signal clicked()

    color: primary ? "white" : "#DDDDDD"

    Image{
        id: staredImage
        height: parent.height*0.6
        width: height

        anchors{
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: parent.height/2
        }

        source: favorite ? "../../graphics/star_on.png" : "../../graphics/star_off.png"

        MouseArea{
            anchors.fill: parent
            onClicked: {
                if(!quickConnectItem.visible) {
                    serverSelectionModel.changeFavorite(serverSelectionItemLine.serverId)
                }
            }
        }
    }

    Image{
        id: countryFlag
        height: parent.height*0.6
        width: visible ? height : 0
        visible: available && serverSelectionItemLine.flag != ""

        source: serverSelectionItemLine.flag != "" ? "https://hide.me/images/flags/png/" + serverSelectionItemLine.flag + ".png" : "https://hide.me/images/flags/png/ru.png"

        anchors{
            verticalCenter: parent.verticalCenter
            left: staredImage.right
            leftMargin: visible ? parent.height/2 : 0
        }
    }

    Colorize {
        anchors.fill: countryFlag
        source: countryFlag
        hue: 0.0
        saturation: 0.0
        lightness: 0.0
        visible: !available
    }

    Item {
        id: clickableZone
        width: childrenRect.width
        height: parent.height

        anchors{
            verticalCenter: parent.verticalCenter
            left: countryFlag.right
            leftMargin: primary ? parent.height/2 : 0
        }

        Text {
            id: countryName
            color: "#555555"
            anchors{
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: parent.height/2
            }
        }

        Image{
            id: downImage
            height: parent.height*0.6
            width: height
            source: serverSelectionItem.implicitHeight == units.gu(6) ? "../../graphics/chevron_down.png" : "../../graphics/chevron_up.png"
            visible: childModel.length > 0 && primary
            anchors{
                verticalCenter: parent.verticalCenter
                left: countryName.right
                leftMargin: parent.height/2
            }
        }

        MouseArea{
            anchors.fill: parent
            onClicked: if(!quickConnectItem.visible) { serverSelectionItemLine.clicked() }
        }
    }

    Image{
        id: isDefault
        height: parent.height*0.6
        width: height

        anchors{
            verticalCenter: parent.verticalCenter
            right: is10gImage.left
            rightMargin: parent.height/2
        }
        visible: cli.defaultHostName == hostname

        source: "../../graphics/location_blue.png"
    }

    Image{
        id: is10gImage
        height: parent.height*0.6
        width: height

        anchors{
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: parent.height/2
        }

        source: "../../graphics/10g.png"
    }

    Rectangle{
        id: downLine
        width: parent.width
        height: 1
        color: "#B0B0B0"

        anchors{
            bottom: parent.bottom
            left: parent.left
        }
    }
}
