import QtQuick 2.0
import QtQuick.Controls.Styles 1.3

SwitchStyle {
    property color backgroundColorChecked: "#e64c3c"
    property color backgroundColorUnchecked: "#ffffff"
    property color backgroundBorderColorChecked: "#e64c3c"
    property color backgroundBorderColorUnchecked: "#dadada"

    groove: Rectangle {
        id: baserect
        implicitWidth: 50 * units.scale
        implicitHeight: 30 * units.scale
        radius: 20 * units.scale
        color: control.checked ? backgroundColorChecked : backgroundColorUnchecked
        border.color: control.checked ? backgroundBorderColorChecked : backgroundBorderColorUnchecked
        border.width: 1 * units.scale
        Behavior on color {ColorAnimation{ duration: 150 }}
        Behavior on border.color {ColorAnimation{ duration: 150 }}
    }

    handle: Item {
        implicitWidth: 30 * units.scale
        implicitHeight: 30 * units.scale

        Rectangle {
            anchors.centerIn: parent
            implicitWidth: 28 * units.scale
            implicitHeight: 28 * units.scale
            radius: 28 * units.scale
            color: "#ffffff"
            border.color: "#dadada"
            border.width: 1 * units.scale
        }
    }
}
