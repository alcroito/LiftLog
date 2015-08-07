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
    signal spreadsheetClicked
    signal doneClicked

    property alias text: label.text
    property alias pixelSize: label.font.pixelSize
    property bool showBackButton: false
    property bool showSettingsButton: false
    property bool showSpreadsheetsButton: false
    property bool showDoneButton: false

    property alias backButton: backButton

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
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
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
            onClicked: spreadsheetClicked()
        }
    }

    Label {
        id: doneButton
        height: root.height
        width: 40 * units.scale
        color: "#ffffff"
        text: qsTr("Done")
        anchors.rightMargin: 10 * units.scale

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.right: parent.right

        font.pixelSize: 12 * units.fontScale
        enabled: showDoneButton
        visible: showDoneButton

        MouseArea {
            anchors.fill: parent
            onClicked: doneClicked()
        }
    }
}

