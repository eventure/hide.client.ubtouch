import QtQuick 2.4
import QtGraphicalEffects 1.12
import Lomiri.Components 1.3

Item {
    id: serverSelectionItem

    property alias favorite: serverSelectionItemLine.favorite
    property alias available: serverSelectionItemLine.available
    property alias flag: serverSelectionItemLine.flag
    property alias countryName: serverSelectionItemLine.countryName
    property alias show10g: serverSelectionItemLine.show10g
    property alias serverId: serverSelectionItemLine.serverId
    property var childModel

    clip: true

    width: parent.width
    implicitHeight: units.gu(6)

    ServerSelectionItemLine{
        id: serverSelectionItemLine
        primary: true
        anchors.top: serverSelectionItem.top

        onClicked: {
            if(childModel.length > 0) {
                if(serverSelectionItem.implicitHeight == units.gu(6)) {
                    serverSelectionItem.implicitHeight = units.gu(6)*( childModel.length + 1)
                } else {
                    serverSelectionItem.implicitHeight = units.gu(6)
                }
            } else {
                serverSelectionModel.activate(serverId)
            }
        }
    }

    Column {
        width: parent.width
        anchors.top: serverSelectionItemLine.bottom

        Repeater{
            id: childRepeater
            width: parent.width
            model: childModel

            delegate: ServerSelectionItemLine{
                primary: false
                serverId: modelData.serverId
                favorite: serverSelectionModel.isFavoriteServer(modelData.serverId)
                countryName: modelData.displayName
                show10g: modelData.tags ? modelData.tags.includes("10g") : false
                onClicked: {
                    serverSelectionModel.activate(modelData.serverId)
                }
            }
        }
    }
}
