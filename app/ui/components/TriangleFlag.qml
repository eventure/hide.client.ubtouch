import QtQuick 2.4
import QtGraphicalEffects 1.0

Item {
    id: triangleFlag
    property alias source: mainImage.source

    Image {
        id: mainImage
        sourceSize: Qt.size(parent.width, parent.height)
        smooth: true
        visible: false
    }

    Image {
        id: mask
        source: "../../graphics/flag_triangle.png"
        sourceSize: Qt.size(parent.width, parent.height)
        smooth: true
        visible: false
    }

    OpacityMask {
        anchors.fill: mainImage
        source: mainImage
        maskSource: mask
    }
}
