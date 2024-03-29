import QtQuick 2.4
import Lomiri.Components 1.3

import "../components"

Page {
    id: loginPage

    header: PageHeader {
        title: "hide.me VPN"
    }

    Label{
        anchors.centerIn: parent
        text: i18n.tr("Can't find hide.me CLI tool")
    }
}
