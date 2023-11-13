import QtQuick 2.4
import Lomiri.Components 1.3

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
        height: parent.height - storeLogButton.height - units.gu(2)
        contentHeight: logText.height
        Text {
            id: logText
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

        text: qsTr("Store logs")
        onClicked: {
            logging.storeToFile()
        }
    }
}
