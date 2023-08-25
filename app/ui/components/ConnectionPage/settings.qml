import QtQuick 2.4
import Lomiri.Components 1.3

import "../"

Rectangle{
    id: settingsArea
    width: parent.width
    height: parent.height
    color: "#F2F2F2"

    Rectangle{
        id: header
        color: "#2AA9E0"
        implicitWidth: units.gu(25)
        implicitHeight: units.gu(10)

        width: parent.width



        Text{
            id: locatioText
            text: qsTr("Settings area")
            color: "#FFF"
            font.pixelSize: units.gu(2)
            font.bold: true
            anchors.centerIn: parent
        }
    }

    ListModel{
        id: settingsListModel
        ListElement { type: "group"; name: qsTr("Service"); }
        ListElement { type: "item"; name: qsTr("Setup VPN service"); action: "page"; page: "../../pages/SetupServicesPage.qml" }
        ListElement { type: "group"; name: qsTr("User"); }
        ListElement { type: "item"; name: qsTr("Quit"); }
        ListElement { type: "item"; name: qsTr("Logout"); }
        ListElement { type: "group"; name: qsTr("About"); }
        ListElement { type: "item"; name: qsTr("Manage accout"); }
        ListElement { type: "item"; name: qsTr("FAQ"); }
        ListElement { type: "item"; name: qsTr("Contact support"); }
        ListElement { type: "item"; name: qsTr("About"); }
    }

    ListView{
        id: settingsListView
        width: parent.width
        height: settingsArea.height - header.height

        anchors{
            top: header.bottom
        }

        model: settingsListModel
        clip: true

        delegate: Rectangle{
            width: parent.width
            implicitHeight: units.gu(5)
            color: type == "group" ? "#F2F2F2" : "#FFFFFF"

            Text{
                text: name
                color: type == "group" ? "#888888" : "#2AA9E0"
                anchors{
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: units.gu(1)
                }
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    if(action == "page") {
                        mainStack.push(Qt.resolvedUrl(page), {})
                    }
                }
            }
        }
    }
}
