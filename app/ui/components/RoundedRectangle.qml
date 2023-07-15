import QtQuick 2.15

Item
{
    id: roundedRectangle
    // As per the regular rectangle
    property color color: "transparent"

    // As per regular rectangle
    property int radius: roundedRectangle.height/4

    // On what side should the corners be shown 5 can be used if no radius is needed.
    // 1 is down, 2 is left, 3 is up and 4 is right.
    property int cornerSide: RoundedRectangle.Direction.None

    property real borderWidth: 0
    property color borderColor: "black"

    enum Direction
    {
        None = 0,
        Down = 1,
        Left = 2,
        Up = 3,
        Right = 4,
        All = 5
    }

    Rectangle
    {
        id: background
        anchors.fill: parent
        radius: cornerSide != RoundedRectangle.Direction.None ? parent.radius : 0
        color: parent.color
        border.width: parent.borderWidth
        border.color: parent.borderColor
    }

    // The item that covers 2 of the corners to make them not rounded.
    Rectangle
    {
        visible: cornerSide != RoundedRectangle.Direction.None && cornerSide != RoundedRectangle.Direction.All
        height: cornerSide % 2 ? parent.radius: parent.height
        width: cornerSide % 2 ? parent.width : parent.radius
        color: parent.color
        anchors
        {
            right: cornerSide == RoundedRectangle.Direction.Left ? parent.right: undefined
            bottom: cornerSide == RoundedRectangle.Direction.Up ? parent.bottom: undefined
        }

        border.width: parent.borderWidth
        border.color: parent.borderColor

        Rectangle
        {
            color: roundedRectangle.color
            height: cornerSide % 2 ? roundedRectangle.borderWidth: roundedRectangle.height - 2 * roundedRectangle.borderWidth
            width: cornerSide % 2 ? roundedRectangle.width - 2 * roundedRectangle.borderWidth: roundedRectangle.borderWidth
            anchors
            {
                right: cornerSide == RoundedRectangle.Direction.Right ? parent.right : undefined
                bottom: cornerSide  == RoundedRectangle.Direction.Down ? parent.bottom: undefined
                horizontalCenter: cornerSide % 2 ? parent.horizontalCenter: undefined
                verticalCenter: cornerSide % 2 ? undefined: parent.verticalCenter
            }
        }
    }
}
