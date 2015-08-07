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
    property bool sideWindowShown: false
    property alias modalPopup: modalPopup
    property alias rootContainer: root
    property alias rootBackground: background
    property bool showNavigationBar: true
    property bool showNavigationBarBackButton: false
    property bool showNavigationBarSettingsButton: false
    property bool showNavigationBarSpreadsheetButton: false
    property bool showNavigationBarDoneButton: false
    default property alias content: innerItem.data
    property bool needToShowModalPopup: false
    property string transitionOrientation: "horizontal"

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
            showDoneButton: showNavigationBarDoneButton
            onSettingsClicked: {
                showSideWindow()
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
            id: sideWindowDragArea
            width: 20 * units.scale

            // Propagate clicks to any elements beneath when drag area is small.
            // When drag area is full screen (side window shown state), the background element is disabled,
            // so clicks won't propogate to, which is what we want.
            propagateComposedEvents: true

            property int startX
            property int bigDelta: appWindow.width * 0.25

            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0

            drag.target: rootBackground
            drag.axis: Drag.XAxis
            drag.minimumX: -appState.windowWidth * 0.85
            drag.maximumX: 0

            enabled: showNavigationBarSettingsButton

            onPressed: {
                startX = mapToItem(null, mouse.x, 0).x

                // Need to disable anchor, so drag works.
                rootBackground.anchors.left = undefined
            }

            onReleased: {
                var deltax = Math.abs(mapToItem(null, mouse.x, 0).x - startX);
                if (deltax > bigDelta && !sideWindowShown) {
                    showSideWindow()
                } else {
                    hideSideWindow()
                }
            }
        }

    }

    SideWindow {
        id: sideWindow
        anchors.fill: parent
        popup: modalPopup
        z: -1

        // Disable it by default, so it doesn't steal mouse events for example
        // when the modal popup is shown.
        enabled: false

        onCloseAndShowPopup: {
            needToShowModalPopup = true
            hideSideWindow()
        }
    }

    ModalPopup {
        id: modalPopup
        enabled: false
        opacity: 0

        function showModalPopup() {
            root.showModalPopup()
        }

        function hideModalPopup() {
            root.hideModalPopup()
        }

        onAcceptClicked: {
            hideModalPopup()
        }

        onRejectClicked: {
            hideModalPopup()
        }
    }

    function showSideWindow() {
        root.state = "sideWindowShown"
        sideWindowShown = true
    }

    function hideSideWindow() {
        root.state = ""
        slideSettingsOutAnimation.start()
        sideWindowShown = false
    }

    function showModalPopup() {
        root.state = "modalPopupShown"
    }

    function hideModalPopup() {
        root.state = ""
    }

    SequentialAnimation {
        id: slideSettingsOutAnimation

        PropertyAnimation {
            target: background
            property: "x"
            to: 0
            duration: 200
        }

        PropertyAction {
            target: background
            property: "anchors.left"
            value: rootContainer.left
        }
    }

    states: [
        State {
            name: ""
            // Reset mouse drag area width, it doesn't
            // get restored to initial value, after an anchor change for some reason.
            PropertyChanges {
                target: sideWindowDragArea
                width: 20 * units.scale
            }
        },
        State {
            name: "sideWindowShown"

            // Disable anchor so that we can move the background to the left.
            AnchorChanges {
                target: background
                anchors.left: undefined
            }

            // Move the background to the left.
            PropertyChanges {
                target: background
                x: -appState.windowWidth * 0.85
            }

            // Make sure to disable mouse events on the navigation bar
            // so that the side window close region works properly.
            PropertyChanges {
                target: navigationBar
                enabled: false
            }

            PropertyChanges {
                target: navigationBar.backButton
                enabled: false
            }

            // Enable side window clicking.
            PropertyChanges {
                target: sideWindow
                enabled: true
            }

            // Make the drag area occupy the whole visible 15% space
            // so that clicking on the visible area, closes the
            // side window.
            AnchorChanges {
                target: sideWindowDragArea
                anchors.left: rootBackground.left
            }
        },
        State {
            name: "modalPopupShown"
            PropertyChanges {
                target: modalPopup
                enabled: true
                opacity: 1
            }

            PropertyChanges {
                target: rootBackground
                enabled: false
            }
        }
    ]

    transitions: [
        Transition {
            from: ""
            to: "sideWindowShown"
            PropertyAnimation {
                property: "x"
                duration: 200
            }
        },
        Transition {
            from: "sideWindowShown"
            to: ""
            PropertyAnimation {
                property: "x"
                duration: 200
            }
            onRunningChanged: {
                if (state == "" && (!running) && needToShowModalPopup) {
                    needToShowModalPopup = false
                    showModalPopup()
                }
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
