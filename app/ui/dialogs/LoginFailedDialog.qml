import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3

Dialog {
    id: loginFailedDialog

    Label {
        text: i18n.tr("Login failed")
    }

    Button {
        text: i18n.tr("Ok")
        onClicked: PopupUtils.close(loginFailedDialog)
    }
}

