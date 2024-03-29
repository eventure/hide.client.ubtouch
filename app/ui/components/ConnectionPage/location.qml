import QtQuick 2.4
import Lomiri.Components 1.3
import hide.me 1.0
import "../"

Rectangle{
    id: locationArea
    width: parent.width
    height: parent.height
    color: "#FFF"

    Rectangle{
        id: header
        color: "#2AA9E0"
        implicitWidth: units.gu(25)
        implicitHeight: units.gu(10)

        width: parent.width

        Text{
            id: locatioText
            text: qsTr("Location selection")
            color: "#FFF"
            font.pixelSize: units.gu(2)
            font.bold: true
            anchors.centerIn: parent
        }
    }

    Rectangle{
        id: popOver
        anchors.fill: parent
        color: "#888888"
        z: 2
        visible: loading.running

        ActivityIndicator {
            id: loading
            anchors.centerIn: parent
            running: serverSelectionModel.count == 0
        }

        Text{
            id: setupText
            text: i18n.tr("Getting servers list...")
            color: "#fff"
            font.pixelSize: units.gu(2)

            anchors{
                top: loading.bottom
                topMargin: units.gu(1)
                horizontalCenter: parent.horizontalCenter
            }
        }
    }

    ListView{
        id: settingsListView
        width: parent.width
        height: locationArea.height - header.height

        visible: serverSelectionModel.count != 0

        anchors{
            top: header.bottom
        }

        clip: true
        model: serverSelectionModel
        delegate: ServerSelectionItem   {
            flag: countryFlag
            countryName: displayName
            show10g: is10g
            serverId: serverIdenty
            favorite: stared
            childModel: childrenServers
        }
    }
}
