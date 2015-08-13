import QtQuick 2.0
import QtQuick.Controls.Styles 1.3

SwitchStyle {
    groove: Rectangle {
        id: baserect
        implicitWidth: 50 * units.scale
        implicitHeight: 25 * units.scale
        radius: 12 * units.scale
        color: control.checked ? "#4bd863" : "#ffffff"
        border.color: "#999"
        border.width: 1 * units.scale
    }

    handle: Item {
        implicitWidth: 29 * units.scale
        implicitHeight: 30 * units.scale

        Rectangle {
            anchors.centerIn: parent
            implicitWidth: 24 * units.scale
            implicitHeight: 23 * units.scale
            radius: 15 * units.scale
            color: "#ffffff"
            border.color: "#dadada"
            border.width: 1 * units.scale
        }
    }
}
