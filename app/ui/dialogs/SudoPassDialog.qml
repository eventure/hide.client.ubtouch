import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3

Dialog {
    id: sudoPassDialog

    Text {
        id: labelText
        text: i18n.tr("Enther administrator password")
        width: passField.width
        height: units.gu(2)
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    TextField{
        id: passField
        echoMode: TextInput.Password
    }

    Button {
        text: i18n.tr("Start service")
        onClicked: {
            cli.startService(passField.text)
        }
    }


    Connections{
        target: cli
        onIsReadyChanged: {
            if(cli.isReady) {
                PopupUtils.close(sudoPassDialog)
            }
        }
    }
}

