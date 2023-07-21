import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3

Dialog {
    id: sudoPassDialog

    Label {
        text: i18n.tr("Enther administrator password")
    }

    TextField{
        id: passField
        echoMode: TextInput.Password
    }

    Button {
        text: cli.connected ? i18n.tr("Dissconnect") : i18n.tr("Connect")
        onClicked: {
            connectionPage.sudoPass = passField.text

            if(cli.connected) {
                cli.disconnecting(connectionPage.sudoPass)
            } else {
                cli.makeConnection(connectionPage.sudoPass)
            }

            PopupUtils.close(sudoPassDialog)
        }
    }
}

