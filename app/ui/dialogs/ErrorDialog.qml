import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3

Dialog {
    id: errorDialog
    property string title;
    property string message;

    Label {
        text: errorDialog.title
    }

    Text {
        id: errorMessage
        text:  mainView.errorMessage

    }

    Button {
        text: i18n.tr("Ok")
        onClicked: PopupUtils.close(errorDialog)
    }
}

