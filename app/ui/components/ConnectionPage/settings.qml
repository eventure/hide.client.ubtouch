import QtQuick 2.4
import Lomiri.Components 1.3

import "../"

Rectangle{
    id: settingsArea
    width: parent.width
    height: parent.height
    color: "#FFF"


    Text{
        id: locatioText
        text: "Settings area"
        color: "#000"
        font.pixelSize: units.gu(2)

        anchors.centerIn: parent
    }
}
