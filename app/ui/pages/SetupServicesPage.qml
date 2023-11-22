import QtQuick 2.4
import Lomiri.Components 1.3
import Lomiri.Components.Extras.PamAuthentication 0.1
import hide.me 1.0
import "../components"

Page {
    id: setupServicesPage

    header: PageHeader {
        title: qsTr("Setup service")
    }

    Component.onCompleted: {
        if(!mainView.fullAccessGranted) {
            authentication.authenticate()
        }
    }

    Column {
        id: loginPageColumn
        width: parent.width - units.gu(4)
        height: parent.height
        anchors{
            top: parent.top
            topMargin: setupServicesPage.header.height + units.gu(4)
            left: parent.left
            leftMargin: units.gu(2)
        }
        spacing: units.gu(4)
        clip: true

        BigButton{
            width: parent.width
            height: units.gu(5)
            color: serviceManager.currentStatus == ServiceManager.NOT_INSTALLED ? "#2AA9E0" : "#A9E02A"
            text: serviceManager.currentStatus == ServiceManager.NOT_INSTALLED ? qsTr("Install service file") : qsTr("Service file installed")

            onClicked: if(serviceManager.currentStatus == ServiceManager.NOT_INSTALLED) { serviceManager.installServies() }
        }

        BigButton{
            id: startOnBootButton
            property bool startOnBoot: serviceManager.startOnBoot
            width: parent.width
            height: units.gu(5)
            visible: serviceManager.currentStatus != ServiceManager.NOT_INSTALLED
            color: startOnBootButton.startOnBoot ? "#A9E02A" : "#2AA9E0"
            text: startOnBootButton.startOnBoot ? qsTr("Disable service on boot") : qsTr("Enable service on boot")

            onClicked:{
                serviceManager.startOnBoot = !serviceManager.startOnBoot
            }
            /*We need timer because QFileSystemWatch not watched on symlink*/
            Timer{
                interval: 500
                running: true
                repeat: true
                onTriggered: startOnBootButton.startOnBoot = serviceManager.startOnBoot
            }
        }

        BigButton{
            width: parent.width
            height: units.gu(5)
            enabled: serviceManager.currentStatus != ServiceManager.NOT_INSTALLED
            color: (serviceManager.currentStatus == ServiceManager.NOT_STARTED || serviceManager.currentStatus == ServiceManager.NOT_INSTALLED) ? "#2AA9E0" : "#A9E02A"
            text:  (serviceManager.currentStatus == ServiceManager.NOT_STARTED || serviceManager.currentStatus == ServiceManager.NOT_INSTALLED) ? qsTr("Start service") : qsTr("Services started")

            onClicked: if(serviceManager.currentStatus == ServiceManager.NOT_STARTED ) {
                           console.log("Do start!")
                           serviceManager.startServie()
                       }
        }


        Component.onCompleted: {
            console.log(serviceManager.currentStatus)
        }
    }
}

