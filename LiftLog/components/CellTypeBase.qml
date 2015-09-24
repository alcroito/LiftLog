import QtQuick 2.0
import QtQuick.Controls 1.4
import LiftLog 1.0

Item {
    id: cellTypeWrapper
    height: 55 * units.scale
    width: appState.windowWidth

    default property alias content: childrenItem.data
    property color cellBackgroundColor: "white"
    clip: true

    property var itemModelData
    property int itemModelIndex: -1
    signal itemIsShowingDeleteButton(int deletingIndex)
    signal itemIsHidingDeleteButton(int deletingIndex)
    signal pleaseHideAllDeleteButtons(int deletingIndex)
    signal pleaseUnfocus()
    property bool itemIsAllowedToShowDeleteButton: true

//            onItemModelDataChanged: {
//                console.log("data", itemModelData, itemModelIndex);
//            }
//            onItemModelIndexChanged: {
//                console.log("index", itemModelData, itemModelIndex);
//            }

    function hideDeleteButton() {
        deleteButton.hide()
    }

    Rectangle {
        id: innerItem
        width: parent.width
        height: parent.height
        color: cellBackgroundColor
        z: 1

        // The children item is animated with a slow pause, after the delete button is hidden completely (it's animation is finished).
        Behavior on x {
            SequentialAnimation {
                PropertyAnimation {
                    property: "x"
                    duration: 100
                }
                PauseAnimation {
                    duration: 100
                }
                ScriptAction {
                    script: {
                        if (!deleteButton.isShown()) {
                            childrenGoToRightAnimation.stop()
                            childrenGoToDefaultStateAnimation.start()
                        }
                    }
                }
            }
        }

        Item {
            id: childrenItem
            width: parent.width
            height: parent.height

            // @TODO Fix this to move more smoothly when sliding open the delete button.
            PropertyAnimation {
                id: childrenGoToDefaultStateAnimation
                target: childrenItem
                property: "x"
                duration: 300
                to: 0
            }

            SmoothedAnimation {
                id: childrenGoToRightAnimation
                target: childrenItem
                property: "x"
                duration: 300
                to: deleteButton.width
            }
        }
    }

    Timer {
        id: deleteDraggerTimer
        interval: 800
        onTriggered: {
            // If dragging hasn't started since clicking on the mouse area,
            // disable the dragging target, and give focus to the item that was positioned at the first click (a text input for example).
            if (!deleteDragger.drag.active) {
                deleteDragger.drag.target = undefined
                deleteDragger.focusInitialMouseClickChild()
            }
        }
    }

    MouseArea {
        id: deleteDragger
        objectName: "deleteDragger"
        anchors.fill: parent
        drag.axis: Drag.XAxis
        z: 2

        property var initialMouseClickEventX
        property var initialMouseClickEventY
        property int deltaXForShowingThreshold: deleteButton.width / 2
        property bool skipOneMouseEvent: false

        drag.target: innerItem
        drag.minimumX: -deleteButton.width
        drag.maximumX: 0
        drag.filterChildren: true
        enabled: itemModelData.deletable ? itemModelData.deletable: false
        propagateComposedEvents: false

        onPressed: {
            // If not allowed to show delete button, it means that another row already shows one, so clicking on this one
            // means all delete buttons should be hidden. So we notify the listView to hide the delete button from another row.
            if (!itemIsAllowedToShowDeleteButton) {
                pleaseHideAllDeleteButtons(itemModelIndex)

                // Because this is not a drag event, but rather a request to hide another row's button, make sure to skip
                // the behavior in the released handler, as well as not enable dragging behavior.
                skipOneMouseEvent = true
                drag.target = undefined
                return;
            }

            // Save initial click position, and start timer to check if the element underneath the mouse should be given focus..
            initialMouseClickEventX = mouse.x
            initialMouseClickEventY = mouse.y
            deleteDraggerTimer.start()
        }
        onCanceled: {
            // When a flickable steals an event, make sure timer does not fire the focus.
            deleteDraggerTimer.stop()

            // Make sure the drag target is restored.
            drag.target = innerItem

            // Make sure to notify list view that we removed the delete button, in case if it was open and we did a flick.
            itemIsHidingDeleteButton(itemModelIndex)

            // Make sure we don't skip any event.
            skipOneMouseEvent = false
        }

        onReleased: {
            // Stop timer, and restore target, if timer was triggered.
            deleteDraggerTimer.stop()
            drag.target = innerItem

            if (skipOneMouseEvent) {
                // If not allowed to show delete button, just skip functionality.
                skipOneMouseEvent = false
                return;
            }

            if (!deleteDragger.drag.active) {
                //  Hide the button if it was shown before, but only if we aren't clicking on the delete button.
                // Otherwise the button would be hidden, and the click would not propagate to the delete button mouse area,
                // because it's enabled property was set to false when the hide() method is called.
                var releasedOverItem = cellTypeWrapper.childAt(mouse.x, mouse.y);
                if (releasedOverItem !== deleteButton) {
                    deleteButton.hide(true)
                } else {
                    // Workaround for MouseAreasand Flickable bug, which required clicking once on a row, before you can
                    // flick or slide the deleted button, which happens when propagateComposedEvents is set to true on the
                    // MouseArea. Solution is to set it to false, and only enable it to true in the onReleased handler.
                    // We need the propagation of the clicked signal, so the deleteButton MouseArea is properly triggered.
                    // http://stackoverflow.com/questions/29236762/mousearea-inside-flickable-is-preventing-it-from-flicking
                    // Ini
                    if (!propagateComposedEvents) {
                        propagateComposedEvents = true
                    }
                }

                // If no drag is active before release, propagate focus to child.
                focusInitialMouseClickChild()
            } else {
                var releaseX = mapToItem(null, mouse.x, 0).x
                var deltax = Math.abs(releaseX - mapToItem(null, initialMouseClickEventX, 0).x);

                // Make sure released position is within window bounds.
                if (deltax > deltaXForShowingThreshold && releaseX > 0 && releaseX < appState.windowWidth) {
                    if (!deleteButton.isShown()) {
                        deleteButton.show()
                    } else {
                        deleteButton.hide()
                    }
                } else {
                    // Not enough delta to change the state, just animate back to how it was.
                    if (deleteButton.isShown()) {
                        deleteButton.show()
                    } else {
                        deleteButton.hide()
                    }
                }
            }
        }
        drag.onActiveChanged: {
            // When a drag is started, animation the childrenItem to go to the right.
            if (drag.active == true) {
                childrenGoToDefaultStateAnimation.stop()
                childrenGoToRightAnimation.start()
            }
        }

        function focusInitialMouseClickChild() {
            var childUnderneathMouseArea = childrenItem.childAt(deleteDragger.initialMouseClickEventX, deleteDragger.initialMouseClickEventY);
            if (childUnderneathMouseArea) {
                childUnderneathMouseArea.forceActiveFocus()
            }
        }
    }

    Rectangle {
        id: deleteButton
        color: "#e74c3c"
        width: cellTypeWrapper.width / 4
        height: cellTypeWrapper.height
        anchors.right: parent.right
        anchors.top: innerItem.top
        z: 0
        enabled: itemModelData.deletable ? itemModelData.deletable: false
        objectName: "deleteButtonRectangle"

        function show() {
            deleteButton.state = "shown"
            innerItem.x = -deleteButton.width
            // Notify listView that a delete button is shown.
            itemIsShowingDeleteButton(itemModelIndex)
        }
        function hide(restoreChildrenItemPositionImmediately) {
            deleteButton.state = ""
            innerItem.x = 0
            // Notify listView that the delete button is hidden.
            itemIsHidingDeleteButton(itemModelIndex)

            // innerItem.x is not set immediately to 0, because there is an animated behavior on it.
            // The only case when it's 0 immediately, is when the dragging operation was ended at coordinate X = 0, in which case
            // we have to manually trigger the animation of childrenItem to go to it's default place, because
            // innerItem's behavior is not executed when the X value is 0 from the dragging operation.
            // We also start the animation when explicitly requested, usually when istead of a drag, just a click is done on the dragging mouse area,
            // to hide the delete button.
            if (innerItem.x == 0 || restoreChildrenItemPositionImmediately) {
                childrenGoToRightAnimation.stop()
                childrenGoToDefaultStateAnimation.start()
            }
        }
        function isShown() {
            return deleteButton.state == "shown"
        }

        states: [
            State {
              name: ""
            },
            State {
                name: "shown"
                PropertyChanges {
                    target: deleteMouseArea
                    enabled: true
                }
            }
        ]

        MouseArea {
            id: deleteMouseArea
            anchors.fill: parent

            onClicked: {
                // Notify listView that a delete button got hidden.
                itemIsHidingDeleteButton(itemModelIndex)

                // Also remove the row.
                settingsModel.removeRow(itemModelIndex)
            }
            // Mouse area is enabled only when delete button is shown.
            enabled: false
        }

        Label {
            id: deleteLabel
            text: qsTr("Delete")
            anchors.centerIn: deleteButton
            font.pixelSize: 12 * units.fontScale
            color: "white"
        }
    }
}

