import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import LiftLog 1.0

Item {
    id: root
    width: appState.windowWidth
    height: childrenRect.height

    property color backgroundColor: "#dfe2e3"
    property color borderColor: "#cacaca"
    property color defaultCellColor: "#ecf0f1"
    property color activeCellColor: "#e74c3c"
    property color grayCellColor: "#e2e2e2"
    property color grayCellTextColor: "#ffffff"
    property var calendarModel

    ListModel {
        id: daysModel
        ListElement {
            name: "Sun"
        }
        ListElement {
            name: "Mon"
        }
        ListElement {
            name: "Tue"
        }
        ListElement {
            name: "Wed"
        }
        ListElement {
            name: "Thu"
        }
        ListElement {
            name: "Fri"
        }
        ListElement {
            name: "Sat"
        }
    }

    Item {
        id: background
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.rightMargin: 13 * units.scale
        anchors.leftMargin: 13 * units.scale
        height: childrenRect.height
        anchors.topMargin: 6 * units.scale

        ListView {
            id: dayLabels
            width: 294
            height: 20 * units.scale
            anchors.left: parent.left
            anchors.leftMargin: 8 * units.scale
            spacing: 15 * units.scale
            orientation: Qt.Horizontal

            anchors.top: parent.top
            anchors.topMargin: 0
            model: daysModel
            delegate: Text {
                text: name
                font.bold: true
                font.pixelSize: 9 * units.fontScale
            }
        }

        GridView {
            id: calendarGrid

            property int sideSize: 40 * units.scale
            property int sideSizeOuter: 42 * units.scale

            width: parent.width
            height: contentItem.childrenRect.height

            anchors.top: dayLabels.bottom
            anchors.topMargin: 10 * units.scale

            model: calendarModel
            cellWidth: sideSizeOuter
            cellHeight: sideSizeOuter

            delegate: Item {
                id: cellOuterBackground
                width: calendarGrid.sideSizeOuter
                height: calendarGrid.sideSizeOuter

                Rectangle {
                    id: cellBackground
                    width: calendarGrid.sideSize
                    height: calendarGrid.sideSize
                    border.color: borderColor
                    anchors.left: parent.left
                    anchors.top: parent.top
                    color: defaultCellColor
                    state: model.state

                    Text {
                        id: cellLabel
                        text: model.text
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    states: [
                        State {
                            name: "empty"
                            PropertyChanges {
                                target: cellBackground
                                opacity: 0
                            }
                        },
                        State {
                            name: "gray"
                            PropertyChanges {
                                target: cellBackground
                                color: grayCellColor
                            }
                            PropertyChanges {
                                target: cellLabel
                                color: grayCellTextColor
                            }
                        },
                        State {
                            name: "active"
                            PropertyChanges {
                                target: cellBackground
                                color: activeCellColor
                            }
                            PropertyChanges {
                                target: cellLabel
                                color: "white"
                            }
                        }
                    ]
                }
            }
        }
    }
}
