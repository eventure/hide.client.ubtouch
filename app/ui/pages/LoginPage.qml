import QtQuick 2.4
import Lomiri.Components 1.3

import "../components"

Page {
    id: loginPage

    header: PageHeader {
        title: "hide.me VPN"
    }

    Flickable {
        id: loginPageFlickable
        clip: true
        flickableDirection: Flickable.AutoFlickIfNeeded
        anchors {
            topMargin: loginPage.header.height + units.gu(1)
            fill: parent
        }
        contentHeight: loginPageColumn.childrenRect.height

        Column {
            id: loginPageColumn
            width: parent.width - units.gu(2)
            spacing: units.gu(3)
            anchors {
                top: parent.top
                topMargin: units.gu(5)
                left: parent.left
                leftMargin: units.gu(1)
            }
            Item {
                id: loginGroup
                width: parent.width
                height: loginButton.height*2

                LoginPassField{
                    id: usernameWrapper
                    width: parent.width
                    height: loginButton.height
                    cornerSide: RoundedRectangle.Direction.Up
                    color: "white"
                    borderWidth: 1
                    borderColor: "#DDDDDD"
                    placeholderText: i18n.tr("Username")
                }

                LoginPassField{
                    id: passwordField
                    width: parent.width
                    height: loginButton.height
                    cornerSide: RoundedRectangle.Direction.Down
                    color: "white"
                    borderWidth: 1
                    borderColor: "#DDDDDD"
                    placeholderText: i18n.tr("Password")
                }
            }
            BigButton{
                id: loginButton
                width: parent.width
                text: qsTr("Login")
            }
        }
    }
}
