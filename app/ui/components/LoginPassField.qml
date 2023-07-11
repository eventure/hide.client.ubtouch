import QtQuick 2.15

RoundedRectangle{
    id: passwordWrapper
    width: parent.width
    height: loginButton.height
    cornerSide: RoundedRectangle.Direction.Down
    color: "white"
    borderWidth: 1
    borderColor: "#DDDDDD"
    property alias placeholderText: placeholderText.text
    property alias echoMode: passwordTextField.echoMode
    property alias text: passwordTextField.text

    anchors{
        top: usernameWrapper.bottom
    }

    TextInput {
        id: passwordTextField
        width: parent.width
        height: parent.height/2
        anchors{
            top: parent.top
            topMargin: parent.height/4
            left: parent.left
            leftMargin: parent.height/4
        }

        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        font.pixelSize:  bigButton.width/20
    }

    Text {
        id: placeholderText
        visible: !passwordTextField.focus

        anchors{
            top: parent.top
            topMargin: parent.height/4
            left: parent.left
            leftMargin: parent.height/4
        }
        color: borderColor
    }
}
