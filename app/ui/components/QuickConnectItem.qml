import QtQuick 2.4

Item{
    id: quickConnectItem
    anchors.fill: parent
    z: 9

    property string flag: "ru"
    property int serverId: 0
    property string hostName: ""
    property bool favorite: false

    Rectangle{
        color: "#888888"
        opacity: 0.7
        anchors.fill: parent
        MouseArea{
            width: parent.width
            height: parent.height - mainItem.height
            anchors{
                top: parent.top
                left: parent.left
            }

            onClicked: {
                quickConnectItem.visible = false
            }
        }
    }

    Rectangle{
        id: header
        color: "#EDF3F5"
        implicitWidth: units.gu(25)
        implicitHeight: units.gu(7)

        width: parent.width
        anchors.bottom: mainItem.top

        Text{
            id: locatioText
            text: qsTr("Quick Connect")
            color: "#000"
            font.pixelSize: units.gu(2)
            anchors{
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: units.gu(2)
            }
        }

        Image{
            id: closeImage
            width: parent.height - units.gu(4)
            height: width
            source: "../../graphics/x.png"

            anchors{
                right: parent.right
                rightMargin: units.gu(2)
                top: parent.top
                topMargin: units.gu(2)
            }

            MouseArea{
                anchors.fill: parent
                onClicked: quickConnectItem.visible = false
            }
        }
    }

    Rectangle {
        id: mainItem
        width: parent.width
        height: units.gu(30)

        anchors.bottom: parent.bottom

        Image {
            id: flagImage
            source: "https://hide.me/images/flags/png/"+flag+".png"
            width: units.gu(6)
            height: width

            anchors{
                top: parent.top
                topMargin: units.gu(2)
                horizontalCenter: parent.horizontalCenter
            }
        }

        Text{
            id: addToFavorites
            text: quickConnectItem.favorite ? qsTr("Remove from favorites") : qsTr("Add to favorites")
            color: "#15749D"
            font.pixelSize: units.gu(2)
            anchors{
                top: flagImage.bottom
                topMargin: units.gu(2)
                horizontalCenter: parent.horizontalCenter
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    serverSelectionModel.changeFavorite(quickConnectItem.serverId)
                    quickConnectItem.favorite = !quickConnectItem.favorite
                }
            }
        }

        Text{
            id: selectDefault
            text: qsTr("Select default")
            color: "#15749D"
            font.pixelSize: units.gu(2)
            anchors{
                top: addToFavorites.bottom
                topMargin: units.gu(2)
                horizontalCenter: parent.horizontalCenter
            }
        }

        BigButton{
            id: connectButton
            width: parent.width - units.gu(4)
            height: units.gu(6)
            text: qsTr("Connect")

            anchors{
                bottom: parent.bottom
                bottomMargin: units.gu(2)
                left: parent.left
                leftMargin: units.gu(2)
            }

            onClicked: {
                cli.setParam("Host", quickConnectItem.hostName)
                cli.makeDisconnection()
                cli.makeConnection()
                bottomLineModel.currentIndex = 0
                quickConnectItem.visible = false
            }
        }

    }
}
