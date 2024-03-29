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
        ListElement { type: "item"; name: qsTr("Store logs"); action: "page"; page: "../../pages/StoreLogsPage.qml" }
        ListElement { type: "item"; name: qsTr("Quit"); action: "script"; doit: "quit"}
        ListElement { type: "item"; name: qsTr("Logout"); action: "script"; doit: "logout"}
        ListElement { type: "group"; name: qsTr("About"); }
        ListElement { type: "item"; name: qsTr("Manage account"); action: "extended"; url: "https://member.hide.me"}
        ListElement { type: "item"; name: qsTr("FAQ"); action: "extended"; url: "https://hide.me/en/knowledgebase/hide-me-vpn-for-ubuntu-touch-faq"}
        ListElement { type: "item"; name: qsTr("Current update"); action: "extended"; url: "https://hide.me/download/ubtouch"}
        ListElement { type: "item"; name: qsTr("About"); action: "extended"; url: "https://hide.me"}
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

                    if(action == "extended") {
                        Qt.openUrlExternally(url)
                    }

                    if(action == "script") {
                        if(doit == "logout") {
                            cli.makeDisconnection()
                            cli.logout();
                        }

                        if(doit == "quit") {
                            cli.quit();
                        }
                    }
                }
            }
        }
    }
}
