import QtQuick 2.0
import QtQuick.Controls 1.2

Rectangle {
    id: root
    width: appState.windowWidth
    height: 40 * units.scale
    color: "#000000"
    z: 5

    signal backClicked
    signal settingsClicked

    property alias text: label.text
    property bool showBackButton: false
    property bool showSettingsButton: false
    property bool showSpreadsheetsButton: false

    FontLoader {
        id: icomoon
        source: "qrc:/assets/fonts/icomoon.ttf"
    }

    Label {
        id: backButton
        width: 40 * units.scale
        height: root.height
        color: "#ffffff"
        text: "\uE605"

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter

        font.family: icomoon.name
        font.pixelSize: 12 * units.fontScale
        enabled: showBackButton
        visible: showBackButton

        MouseArea {
            anchors.fill: parent
            onClicked: backClicked()
        }
    }

    Text {
        id: label
        color: "#ffffff"
        text: "LIFT LOG"
        anchors.bottomMargin: 10 * units.scale
        anchors.topMargin: 8 * units.scale
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        font.bold: true
        z: 1
        font.pixelSize: 14 * units.fontScale
    }

    Label {
        id: settingsButton
        height: root.height
        width: 40 * units.scale
        color: "#ffffff"
        text: "\uE20E"

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.right: parent.right

        font.family: icomoon.name
        font.pixelSize: 12 * units.fontScale
        enabled: showSettingsButton
        visible: showSettingsButton

        MouseArea {
            anchors.fill: parent
            onClicked: settingsClicked()
        }
    }

    Label {
        id: spreadsheetsButton
        height: root.height
        width: 40 * units.scale
        color: "#ffffff"
        text: "\uF278"

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.right: parent.right

        font.family: icomoon.name
        font.pixelSize: 12 * units.fontScale
        enabled: showSpreadsheetsButton
        visible: showSpreadsheetsButton

        MouseArea {
            anchors.fill: parent
            onClicked: settingsClicked()
        }
    }
}

