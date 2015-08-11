import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import LiftLog 1.0
import "../components"

Item {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight

    property bool sideWindowShown: false
    property alias sideWindow: sideWindowLoader.item
    property alias sideWindowLoader: sideWindowLoader

    property alias modalPopup: modalPopupLoader.item
    property alias modalPopupLoader: modalPopupLoader

    property alias datePickerDialog: datePickerDialogLoader.item
    property alias datePickerDialogLoader: datePickerDialogLoader

    property alias rootContainer: root
    property alias rootBackground: background

    property alias navigationBar: navigationBar
    property bool showNavigationBar: true
    property bool showNavigationBarBackButton: false
    property bool showNavigationBarSettingsButton: false
    property bool showNavigationBarSpreadsheetButton: false
    property bool showNavigationBarDoneButton: false

    default property alias content: innerItem.data

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
            // so clicks won't propogate to it, which is what we want.
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

    Loader {
        id: sideWindowLoader
        active: showNavigationBarSettingsButton
        z: -1
        sourceComponent: SideWindow {
            id: sideWindow
            sideWindowDragAreaReference: sideWindowDragArea
            rootBackgroundReference: rootBackground
            rootContainerReference: rootContainer
            navigationBarReference: navigationBar
            innerItemReference: innerItem

            // Disable it by default, so it doesn't steal mouse events for example
            // when the modal popup is shown.
            enabled: false

            onCloseAndShowPopupForOperation: {
                function showPopupHandler() {
                    // Disconnect the slot, so it's a one-time fire event.
                    sideWindow.sideWindowHidingComplete.disconnect(showPopupHandler)

                    // Show popup.
                    showModalPopup()
                    modalPopup.prepare(op)
                }

                // Connect to the sideWindowHidingComplete signal, to show the popup when it emits.
                sideWindow.sideWindowHidingComplete.connect(showPopupHandler)

                // Hide the window.
                hideSideWindow()
            }
        }
    }

    Loader {
        id: modalPopupLoader
        active: false
        sourceComponent: ModalPopup {
            id: modalPopup
            disableComponent: rootBackground
            onAcceptClicked: hideModalPopup()
            onRejectClicked: hideModalPopup()
        }
    }

    Loader {
        id: datePickerDialogLoader
        active: false
        property date initialDate

        sourceComponent: DatePickerDialog {
            id: datePickerDialog
            initialDate: datePickerDialogLoader.initialDate
            disableComponent: rootBackground
            onRejectClicked: hideDatePicker()
            onAcceptClicked: hideDatePicker()
        }
    }

    function showSideWindow() {
        sideWindow.state = "sideWindowShown"
        sideWindowShown = true
    }

    function hideSideWindow() {
        sideWindow.state = ""
        sideWindow.slideSettingsOutAnimation.start()
        sideWindowShown = false
    }

    function showModalPopup() {
        modalPopupLoader.active = true
        modalPopup.state = "modalPopupShown"
    }

    function hideModalPopup() {
        modalPopup.state = ""
    }

    function showDatePicker() {
        datePickerDialogLoader.active = true
        datePickerDialogLoader.item.state = "datePickerShown"
    }

    function hideDatePicker() {
        datePickerDialogLoader.item.state = ""
    }
}
