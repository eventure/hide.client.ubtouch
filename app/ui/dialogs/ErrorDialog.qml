import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3

Dialog {
    id: errorDialog
    property string title;
    property string message;

    Label {
        id: errorTitle
        text: errorDialog.title
        font.bold: true
    }

    Label {
        id: errorMessage
        text: errorDialog.message
        width: parent.width
        clip: true
        wrapMode: Text.WordWrap
    }

    Button {
        text: i18n.tr("Ok")
        onClicked: PopupUtils.close(errorDialog)
    }
}

