import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import LiftLog 1.0
import "../components"

Item {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight

    property color iconColor: "#e94c3a"
    property alias slideSettingsOutAnimation: slideSettingsOutAnimation
    property var sideWindowDragAreaReference: null
    property var rootBackgroundReference: null
    property var rootContainerReference: null
    property var navigationBarReference: null
    property var innerItemReference: null

    signal close
    signal closeAndShowPopupForOperation(string op)
    signal sideWindowHidingComplete

    Rectangle {
        id: background
        color: "#000000"
        anchors.fill: parent

        Item {
            id: shiftedContainer
            anchors.topMargin: 50 * units.scale
            anchors.fill: parent
            anchors.leftMargin: appState.windowWidth * 0.15

            ListModel {
                id: sideWindowModel
                ListElement {
                    text: qsTr("Switch to next workout day")
                    icon: "\uea45"
                    op: "next"
                }
                ListElement {
                    text: qsTr("Switch to previous workout day")
                    icon: "\uea45"
                    op: "prev"
                }
                ListElement {
                    text: qsTr("Delete Workout")
                    icon: "\ue9ac"
                    op: "delete"
                }
                ListElement {
                    text: qsTr("Switch Sets & Reps")
                    icon: "\ue2fd"
                    op: "switch"
                }
                ListElement {
                    text: qsTr("Settings")
                    icon: "\ue994"
                    op: "settings"
                }
                ListElement {
                    text: qsTr("About")
                    icon: "\uea0c"
                    op: "about"
                }
            }

            FontLoader {
                id: icomoon
                source: "qrc:/assets/fonts/icomoon.ttf"
            }

            ListView {
                id: sideWindowList
                width: childrenRect.width
                height: childrenRect.height
                anchors.left: parent.left
                anchors.leftMargin: 10 * units.scale
                model: sideWindowModel
                spacing: 10 * units.scale
                delegate: Item {
                    id: itemContainer
                    width: rowLayout.width + 10 * units.scale
                    height: rowLayout.height  + 25 * units.scale

                    Row {
                        id: rowLayout
                        anchors.centerIn: itemContainer

                        Text {
                            id: icon
                            font.family: icomoon.name
                            text: model.icon
                            font.pixelSize: 12 * units.fontScale
                            color: iconColor
                        }

                        Item {
                            id: spacingItem
                            width: 12 * units.scale
                            height: 1
                        }

                        Label {
                            height: icon.height
                            id: textLabel
                            text: model.text
                            verticalAlignment: Text.AlignVCenter
                            color: "white"
                            font.pixelSize: 10 * units.fontScale
                            font.weight: Font.Light
                        }
                    }

                    MouseArea {
                        id: clickArea
                        anchors.fill: itemContainer
                        onClicked: {
                            switch (model.op) {
                                case "delete":
                                    closeAndShowPopupForOperation(model.op)
                            }
                        }
                    }
                }
            }
        }
    }

    SequentialAnimation {
        id: slideSettingsOutAnimation

        PropertyAnimation {
            target: rootBackgroundReference
            property: "x"
            to: 0
            duration: 200
        }

        PropertyAction {
            target: rootBackgroundReference
            property: "anchors.left"
            value: rootContainerReference.left
        }
    }

    states: [
        State {
            name: ""
            // Reset mouse drag area width, it doesn't
            // get restored to initial value, after an anchor change for some reason.
            PropertyChanges {
                target: sideWindowDragAreaReference
                width: 20 * units.scale
            }
        },
        State {
            name: "sideWindowShown"

            // Disable anchor so that we can move the background to the left.
            AnchorChanges {
                target: rootBackgroundReference
                anchors.left: undefined
            }

            // Move the background to the left.
            PropertyChanges {
                target: rootBackgroundReference
                x: -appState.windowWidth * 0.85
            }

            // Make sure to disable mouse events on the navigation bar
            // so that the side window close region works properly, so
            // the click event goes to the sideWindowDragArea region to close
            // the side window, rather than to the navigation bar
            // which will try to open the window again.
            PropertyChanges {
                target: navigationBarReference
                enabled: false
            }

            // Disable the content area as well.
            PropertyChanges {
                target: innerItemReference
                enabled: false
            }

            // Enable side window clicking.
            PropertyChanges {
                target: root
                enabled: true
            }

            // Make the drag area occupy the whole visible 15% space
            // so that clicking on the visible area, closes the
            // side window.
            AnchorChanges {
                target: sideWindowDragAreaReference
                anchors.left: rootBackgroundReference.left
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

            // Animate all enabled property changes to occur at the end of the transition.
            // When a boolean value like this is animated, it is implied it will change its
            // value at the end of the transition.
            PropertyAnimation {
                property: "enabled"
            }

            onRunningChanged: {
                if (!running) {
                    sideWindowHidingComplete()
                }
            }
        }
    ]
}
