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

    signal movedToPrevMonth(var newDate)
    signal movedToNextMonth(var newDate)
    signal dateClicked(var date)

    function goPrevMonth() {
        var date = calendarSliderModel.goPrevMonth()
        calendarList.positionViewAtIndex(1, ListView.Center)
        calendarList.currentIndex = 1
        return date
    }

    function goNextMonth() {
        var date = calendarSliderModel.goNextMonth()
        calendarList.positionViewAtIndex(1, ListView.Center)
        calendarList.currentIndex = 1
        return date
    }

    function refresh() {
        // Refreshes the current month list of workouts.
        var calendarGridInstance = calendarList.currentItem.getGridView().getModel().refresh()
    }

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
        anchors.topMargin: 10 * units.scale

        ListView {
            id: dayLabels
            width: contentItem.childrenRect.width
            height: 20 * units.scale
            interactive: false
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 0
            orientation: Qt.Horizontal

            anchors.top: parent.top
            anchors.topMargin: 0
            model: daysModel
            delegate: Item {
                width: calendarList.sideSizeOuter

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: name
                    font.bold: true
                    font.pixelSize: 9 * units.fontScale
                }
            }
        }
    }

    CalendarSliderModel {
        id: calendarSliderModel
    }

    ListView {
        id: calendarList
        orientation: Qt.Horizontal
        spacing: 0
        highlightFollowsCurrentItem: false

        width: parent.width
        height: contentItem.childrenRect.height
        flickDeceleration: 1000
        maximumFlickVelocity: 1000
        boundsBehavior: Flickable.DragOverBounds

        anchors.top: background.bottom
        anchors.topMargin: 10 * units.scale
        anchors.horizontalCenter: parent.horizontalCenter

        snapMode: ListView.SnapOneItem

        clip: true
        model: calendarSliderModel

        property int sideSize: 40 * units.scale
        property int sideSizeOuter: 42 * units.scale

        Component.onCompleted: {
            calendarList.currentIndex = 1
            calendarList.positionViewAtIndex(1, ListView.Center)
        }

        onMovementEnded: {
            // Set the currentIndex to the item that is currently visible in the listview.
            calendarList.currentIndex = calendarList.indexAt(calendarList.contentX, calendarList.contentY);

            // If the item is not the middle one, we move to the previous or next month.
            if (calendarList.currentIndex == 0) {
                root.movedToPrevMonth(goPrevMonth())
            }
            if (calendarList.currentIndex == 2) {
                root.movedToNextMonth(goNextMonth())
            }
        }

        delegate: Row {
            Item {
                id: beforeSpacing
                width: (calendarList.width - calendarList.sideSizeOuter * 7) / 2
                height: 1
            }

            function getGridView() {
                return calendarGrid
            }

            GridView {
                id: calendarGrid

                width: calendarList.sideSizeOuter * 7
                height: contentItem.childrenRect.height

                model: monthModel
                cellWidth: calendarList.sideSizeOuter
                cellHeight: calendarList.sideSizeOuter
                interactive: false

                function getModel() {
                    return model
                }

                delegate: Item {
                    id: cellOuterBackground
                    width: calendarList.sideSizeOuter
                    height: calendarList.sideSizeOuter

                    Rectangle {
                        id: cellBackground
                        width: calendarList.sideSize
                        height: calendarList.sideSize
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

                        MouseArea {
                            id: cellClickArea
                            anchors.fill: parent
                            property string initialState

                            onPressed: {
                                initialState = cellBackground.state
                            }

                            onPressAndHold: {
                                if (initialState != "gray" && initialState != "active" && initialState != "empty") {
                                    cellBackground.state = "hold"
                                }
                            }

                            onReleased: {
                                if (cellBackground.state == "hold") {
                                    cellBackground.state = initialState
                                    cellClickArea.clicked(mouse)
                                }
                            }

                            onClicked: {
                                if (cellBackground.state != "empty" && cellBackground.state != "gray")
                                dateClicked(model.date)
                            }
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
                                name: "hold"
                                PropertyChanges {
                                    target: cellBackground
                                    color: Qt.lighter(activeCellColor)
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

                        transitions: [
                            Transition {
                                from: ""
                                to: "hold"

                                ColorAnimation {
                                    target: cellBackground
                                    duration: 400
                                }
                            },
                            Transition {
                                from: "hold"
                                to: ""

                                ColorAnimation {
                                    target: cellBackground
                                    duration: 400
                                }
                            }
                        ]
                    }
                }
            }

            Item {
                id: afterSpacing
                width: (calendarList.width - calendarList.sideSizeOuter * 7) / 2
                height: 1
            }
        }
    }
}
