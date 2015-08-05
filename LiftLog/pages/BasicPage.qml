import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import LiftLog 1.0
import "../components"

Item {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight

    property Component pageComponent
    property alias navigationBar: navigationBar
    property alias sideWindow: sideWindow
    property alias modalPopup: modalPopup
    property bool showNavigationBar: true
    property bool showNavigationBarBackButton: false
    property bool showNavigationBarSettingsButton: false
    property bool showNavigationBarSpreadsheetButton: false
    default property alias content: innerItem.data

    signal goBack

    Rectangle {
        id: background
        width: parent.width
        height: parent.height
        anchors.left: root.left
        color: "#ecf0f1"

        NavigationBar {
            id: navigationBar
            anchors.top: parent.top
            anchors.topMargin: 0
            width: parent.width
            opacity: showNavigationBar ? 1 : 0
            showBackButton: showNavigationBarBackButton
            showSettingsButton: showNavigationBarSettingsButton
            showSpreadsheetsButton: showNavigationBarSpreadsheetButton
            onSettingsClicked: {
                showSettings()
            }
        }

        Item {
            id: innerItem
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.top: navigationBar.bottom
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
        }

        MouseArea {
            id: sideWindowCloseRegion
            onClicked: root.state = ""
            anchors.fill: parent
            enabled: root.state == "sideWindowShown"
        }
    }

    SideWindow {
        id: sideWindow
        anchors.left: parent.right
        anchors.leftMargin: 0
    }

    ModalPopup {
        id: modalPopup
        enabled: false
        opacity: 0

        onAcceptClicked: {
            root.state = ""
        }

        onRejectClicked: {
            root.state = ""
        }
    }

    function showSettings() {
        root.state = "sideWindowShown"
    }

    states: [
        State {
            name: "sideWindowShown"
            PropertyChanges {
                target: sideWindow
                anchors.leftMargin: -appState.windowWidth * 0.85
            }
            PropertyChanges {
                target: background
                anchors.leftMargin: -appState.windowWidth * 0.85
            }

            // Make sure to disable mouse events on the navigation bar
            // so that the side window close region works properly.
            PropertyChanges {
                target: navigationBar
                enabled: false
            }
        },
        State {
            name: "modalPopupShown"
            PropertyChanges {
                target: modalPopup
                enabled: true
                opacity: 1
            }
        }
    ]

    transitions: [
        Transition {
            from: ""
            to: "sideWindowShown"
            PropertyAnimation {
                property: "leftMargin"
                duration: 200
            }
        },
        Transition {
            from: "sideWindowShown"
            to: ""
            PropertyAnimation {
                property: "leftMargin"
                duration: 100
            }
        },
        Transition {
            from: ""
            to: "modalPopupShown"
            PropertyAnimation {
                property: "opacity"
                easing.type: Easing.InQuad
                duration: 100
            }
        },
        Transition {
            from: "modalPopupShown"
            to: ""
            PropertyAnimation {
                property: "opacity"
                duration: 100
            }
        }
    ]
}
