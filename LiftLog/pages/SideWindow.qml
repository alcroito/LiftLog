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
    property var popup

    signal close
    signal closeAndShowPopup

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
                                case "delete": {
                                    root.closeAndShowPopup()
                                    popup.prepareDeleteWorkout(model.op)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
