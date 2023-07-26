import QtQuick 2.4

Rectangle {
    id: bigButton

    property bool inverted: false
    property alias text: textButton.text
    signal clicked();

    color: inverted ? "#FFFFFF" : "#2AA9E0"

    implicitWidth: units.gu(25)
    implicitHeight: units.gu(5)

    radius: height/8

    Text{
        id: textButton
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize:  bigButton.height/3
        color: inverted ? "#2AA9E0" : "#FFFFFF"
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            bigButton.clicked()
        }
    }
}
