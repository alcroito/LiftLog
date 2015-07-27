import QtQuick 2.0
import QtQuick.Controls.Styles 1.3

SwitchStyle {
    groove: Rectangle {
        id: baserect
        implicitWidth: 50
        implicitHeight: 25
        radius: 12
        color: control.checked ? "#4bd863" : "#ffffff"
        border.color: "#999"
        border.width: 1
    }

    handle: Item {
        implicitWidth: 29
        implicitHeight: 30

        Rectangle {
            anchors.centerIn: parent
            implicitWidth: 24
            implicitHeight: 23
            radius: 15
            color: "#ffffff"
            border.color: "#dadada"
            border.width: 1
        }
    }
}
