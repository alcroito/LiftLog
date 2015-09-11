import QtQuick 2.0
import QtQuick.Controls 1.2

Rectangle {
    id: root
    width: appState.windowWidth
    height: 40 * units.scale
    color: "#000000"
    z: 5

    signal backClicked
    signal burgerClicked
    signal spreadsheetClicked
    signal doneClicked
    signal warmupClicked
    signal leftGearClicked
    signal resetClicked

    property alias text: label.text
    property alias pixelSize: label.font.pixelSize
    property alias bold: label.font.bold
    property bool showBackButton: false
    property bool showBurgerButton: false
    property bool showSpreadsheetsButton: false
    property bool showDoneButton: false
    property bool showWarmupButton: false
    property bool showLeftGearButton: false
    property bool showRightResetButton: false

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
        Accessible.ignored: true

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
        id: burgerButton
        height: root.height
        width: 40 * units.scale
        color: "#ffffff"
        text: "\uE20E"

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.right: parent.right

        font.family: icomoon.name
        font.pixelSize: 12 * units.fontScale
        enabled: showBurgerButton
        visible: showBurgerButton
        Accessible.ignored: true

        MouseArea {
            anchors.fill: parent
            onClicked: burgerClicked()
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
        Accessible.ignored: true

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
        Accessible.ignored: true

        MouseArea {
            anchors.fill: parent
            onClicked: doneClicked()
        }
    }

    Label {
        id: warmupButton
        height: root.height
        width: 40 * units.scale
        color: "#ffffff"
        text: "\uE9A9"
        anchors.rightMargin: warmupButton.width

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.right: parent.right

        font.family: icomoon.name
        font.pixelSize: 12 * units.fontScale
        enabled: showWarmupButton
        visible: showWarmupButton
        Accessible.ignored: true

        MouseArea {
            anchors.fill: parent
            onClicked: warmupClicked()
        }
    }

    Label {
        id: leftGearButton
        height: root.height
        width: 40 * units.scale
        color: "#ffffff"
        text: "\uE994"

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter

        font.family: icomoon.name
        font.pixelSize: 12 * units.fontScale
        enabled: showLeftGearButton
        visible: showLeftGearButton
        Accessible.ignored: true

        MouseArea {
            anchors.fill: parent
            onClicked: leftGearClicked()
        }
    }

    Label {
        id: rightResetButton
        height: root.height
        width: 40 * units.scale
        color: "#ffffff"
        text: "\ue611"

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.right: parent.right

        font.family: icomoon.name
        font.pixelSize: 12 * units.fontScale
        enabled: showRightResetButton
        visible: showRightResetButton
        Accessible.ignored: true

        MouseArea {
            anchors.fill: parent
            onClicked: resetClicked()
        }
    }
}

