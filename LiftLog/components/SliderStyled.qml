import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Slider {
    id: slider

    style: SliderStyle {
        groove: Rectangle {
            implicitWidth: 200 * units.scale
            implicitHeight: 2 * units.scale
            color: "#e74c3c"
            radius: 50 * units.scale

            Rectangle {
                id: grayLine
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: styleData.handlePosition
                anchors.rightMargin: 0
                implicitHeight: 2 * units.scale
                color: "#e8e8e8"
                radius: 50 * units.scale
            }
        }
        handle: Rectangle {
            anchors.centerIn: parent
            color: "white"
            border.color: "#e8e8e8"
            border.width: 1 * units.scale
            implicitWidth: 28 * units.scale
            implicitHeight: 28 * units.scale
            radius: 28 * units.scale
        }
    }
}

