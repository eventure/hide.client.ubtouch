import QtQuick 2.15

Rectangle {
    id: bigButton


    property bool inverted: false
    property alias text: textButton.text
    signal clicked();

    color: inverted ? "#FFFFFF" : "#2AA9E0"

    implicitWidth: units.gu(25)
    implicitHeight: units.gu(5)

    radius: height/4

    Text{
        id: textButton
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize:  bigButton.width/20
        color: inverted ? "#2AA9E0" : "#FFFFFF"
    }

    MouseArea{
        anchors.fill: parent
        onClicked: bigButton.clicked()
    }
}
