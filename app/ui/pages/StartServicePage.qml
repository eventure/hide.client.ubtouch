import QtQuick 2.4
import Lomiri.Components 1.3

import "../components"

Page {
    id: startServicePage

    header: PageHeader {
        title: "hide.me VPN"
    }

    Rectangle{
        id: loadingArea
        anchors.fill: startServicePage
        color: "#888888"
        z: 9999
        visible: loading.running

        ActivityIndicator {
            id: loading
            anchors.centerIn: parent
            running: false
        }

        Text{
            id: setupText
            text: i18n.tr("Starting service...")
            color: "#fff"
            font.pixelSize: units.gu(2)

            anchors{
                top: loading.bottom
                topMargin: units.gu(1)
                horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Rectangle {
        anchors.fill: startServicePage
        visible: !loading.running

        Text {
            id: labelText
            text: i18n.tr("Enther administrator password")
            width: parent.width
            height: units.gu(2)
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            anchors{
                bottom: passField.top
                bottomMargin: units.gu(2)
            }
        }

        TextField{
            id: passField
            echoMode: TextInput.Password
            anchors.centerIn: parent
        }

        Button {
            text: i18n.tr("Start service")
            anchors{
                bottom: parent.bottom
                bottomMargin: units.gu(2)
                horizontalCenter: parent.horizontalCenter
            }
            //enabled: passField.text.length > 5

            onClicked: {
                if(passField.text.length > 5) {
                    loading.running = true
                    cli.startService(passField.text)
                }
            }
        }
    }
}
