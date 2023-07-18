import QtQuick 2.4

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
        font.pixelSize: passwordTextField.height*0.8
    }

    Text {
        id: placeholderText
        visible: !passwordTextField.focus && passwordTextField.text.length  == 0

        anchors{
            top: parent.top
            topMargin: parent.height/4
            left: parent.left
            leftMargin: parent.height/4
        }
        color: borderColor
    }
}
