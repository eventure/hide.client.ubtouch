import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3

import "../components"

Page {
    id: startServicePage

    header: PageHeader {
        title: qsTr("Logs")
    }

    Connections{
        target: logging
        onEntryAdded:{
            logText.text = logging.getLogString()
            if(logText.height > logArea.height) {
                logArea.contentY = logText.height - logArea.height
            }
        }
    }

    Flickable{
        id: logArea
        width: parent.width
        height: parent.height - storeLogButton.height - units.gu(8)
        contentHeight: logText.height

        Text {
            id: logText
            text: logging.getLogString()
        }

        Text{
            id: emptyLogText
            text: qsTr("Logs is empty")
            anchors.centerIn: parent
            visible: logText.text.length == 0
        }

        anchors{
            bottom: storeLogButton.top
            bottomMargin: units.gu(2)
        }
    }

    BigButton{
        id: storeLogButton
        width: parent.width - units.gu(4)
        anchors{
            left: parent.left
            leftMargin: units.gu(2)
            bottom: parent.bottom
            bottomMargin: units.gu(2)
        }

        text: qsTr("Save log file")
        onClicked: {
            cli.storeLogsToFile()
            PopupUtils.open(Qt.resolvedUrl("../dialogs/ErrorDialog.qml"), startServicePage,
                                {
                                    title: qsTr("Logs saved"),
                                    message: qsTr("Application log saved to Documents directory")
                                })
        }
    }
}
