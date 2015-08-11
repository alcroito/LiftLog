import QtQuick 2.0
import QtQuick.Controls 1.2
import LiftLog 1.0

Item {
    id: root

    property int outerSideLength: 45
    property color redColor: "#e1352d"
    property color whiteColor: "#ffffff"
    property color borderColor: "#cccccc"
    property color crossedColor: "#f6f6f6"
    property color crossedStrokeColor: "#a0a0a0"

    width: outerSideLength * units.scale
    height: outerSideLength * units.scale

    property alias text: label.text
    property int repsDone: 5
    property int repsToDo: 5
    property var setModelIndex
    signal clicked(int repsDone, int repsToDo)

    state: "empty"

    Rectangle {
        id: circle
        width: outerSideLength * units.scale
        height: outerSideLength * units.scale
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        radius: (outerSideLength * units.scale) / 2
        border.width: 1 * units.scale
        border.color: borderColor
        color: whiteColor
        SequentialAnimation {
            id: blinkingAnimation

            // Before infinitely blinking, wait for a small bit of time,
            // so that when the workout page loads, the animation isn't half way done.
            PauseAnimation {
                duration: 500
            }

            SequentialAnimation {
                loops: Animation.Infinite
                ColorAnimation {
                    target: circle
                    property: "color"

                    easing.type: Easing.Linear
                    from: whiteColor
                    to: redColor
                    duration: 1000
                }
                ColorAnimation {
                    target: circle
                    property: "color"
                    easing.type: Easing.Linear
                    from: redColor
                    to: whiteColor
                    duration: 1000
                }
            }
        }

        Label {
            id: label
            text: "5x5"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {

                // Use additional variables, so no accidental modification is done to the component bindings.
                var newState = root.state
                var newRepsDone = repsDone
                var newRepsToDo = repsToDo

                if (newState == "empty" || newState == "blinking") {
                    newState = "active"
                    newRepsDone = newRepsToDo
                } else if (root.state == "active") {
                    newRepsDone = newRepsDone - 1
                    if (newRepsDone == -1) {
                        newState = "empty"
                    }
                }

                // Set the model values, which will in turn modify this component's values, because of the bindings
                // done to the model values.
                appState.currentWorkoutModel.setData(setModelIndex, newRepsDone, WorkoutModel.RepsDoneCountRole)
                appState.currentWorkoutModel.setData(setModelIndex, newRepsToDo, WorkoutModel.RepsToDoCountRole)
                appState.currentWorkoutModel.setData(setModelIndex, newState, WorkoutModel.RepsSetStateRole)

                // Signal parent about this set being clicked.
                if (newState != "crossed" ) {
                    root.clicked(repsDone, repsToDo)
                }
            }
        }

        Canvas {
            id: cross

            property int sideLength: 16 * units.scale

            width: sideLength
            height: sideLength
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            opacity: 0

            onPaint: {
                var ctx = cross.getContext('2d')
                ctx.lineWidth = 1.5 * units.scale
                ctx.strokeStyle = crossedStrokeColor

                // Left-top to bottom-right cross line
                ctx.beginPath()
                ctx.moveTo(0, 0)
                ctx.lineTo(sideLength, sideLength)
                ctx.closePath()
                ctx.stroke()

                // Right-top to bottom-left cross line
                ctx.beginPath()
                ctx.moveTo(sideLength, 0)
                ctx.lineTo(0, sideLength)
                ctx.closePath()
                ctx.stroke()
            }
        }
    }
    states: [
        State {
            name: "empty"
            PropertyChanges { target: label; text: "" }
            PropertyChanges { target: circle; color: whiteColor }
        },
        State {
            name: "standard"
        },
        State {
            name: "active"
            PropertyChanges { target: circle; color: redColor }
            PropertyChanges { target: label; color: whiteColor }
        },
        State {
            name: "crossed"
            PropertyChanges { target: label; text: "" }
            PropertyChanges { target: circle; border.width: 0; color: crossedColor }
            PropertyChanges { target: cross; opacity: 1 }
        },
        State {
            name: "blinking"
            PropertyChanges { target: label; text: "" }
            PropertyChanges { target: blinkingAnimation; running: true }
        }
    ]
}

