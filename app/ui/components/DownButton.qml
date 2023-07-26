import QtQuick 2.4

Item {
    id: downButton
    width: parent.width/3
    height: parent.height

    property alias text: downButtonText.text
    property bool active: false

    signal clicked()

    Text{
        id: downButtonText
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: downButton.active ? "#2AA9E0" : "#888888"
    }

    MouseArea{
        anchors.fill: parent
        onClicked: downButton.clicked()
    }
}
