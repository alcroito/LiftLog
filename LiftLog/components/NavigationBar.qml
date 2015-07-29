import QtQuick 2.0
import QtQuick.Controls 1.2

Rectangle {
    id: root
    width: appState.windowWidth
    height: 40 * units.scale
    color: "#000000"
    z: 5
    signal backClicked
    property alias text: label.text
    property bool showBackButton: false

    FontLoader {
        id: icomoon
        source: "qrc:/assets/fonts/icomoon.ttf"
    }

    Label {
        id: backButton
        color: "#ffffff"
        text: "\uE605"
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        font.family: icomoon.name
        font.pixelSize: units.sp(12)
        opacity: showBackButton ? 1 : 0

        MouseArea {
            anchors.fill: parent
            onClicked: backClicked()
        }
    }

    Text {
        id: label
        color: "#ffffff"
        text: "LIFT LOG"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 12
        anchors.top: parent.top
        anchors.topMargin: 12
        font.bold: true
        z: 1
        font.pixelSize: units.sp(14)
    }
}

