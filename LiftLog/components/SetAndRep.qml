import QtQuick 2.0
import QtQuick.Controls 1.2

Item {
    id: root

    property int outerSideLength: 45

    width: outerSideLength * units.scale
    height: outerSideLength * units.scale

    property alias text: label.text
    property int reps: 5
    property int repsToDo: 5
    state: "empty"
    signal clicked(int reps, int repsToDo)

    Rectangle {
        id: circle
        width: outerSideLength * units.scale
        height: outerSideLength * units.scale
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        radius: (outerSideLength * units.scale) / 2
        border.width: 1 * units.scale
        border.color: Qt.darker("#dadada")
        color: "#ffffff"

        Label {
            id: label
            text: "5x5"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (root.state == "empty") {
                    root.state = "active"
                    reps = repsToDo
                    label.text = reps
                } else if (root.state == "active") {
                    reps = reps - 1
                    if (reps == -1) {
                        root.state = "empty"
                    } else {
                        label.text = reps
                    }
                }
                root.clicked(reps, repsToDo)
            }
        }

        Canvas {
            id: cross

            property int sideLength: 14 * units.scale

            width: sideLength
            height: sideLength
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            opacity: 0

            onPaint: {
                var ctx = cross.getContext('2d')
                ctx.lineWidth = 1
                ctx.strokeStyle = Qt.darker("gray")

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
        },
        State {
            name: "standard"
        },
        State {
            name: "active"
            PropertyChanges { target: circle; color: "#e1352d" }
            PropertyChanges { target: label; color: "white" }
        },
        State {
            name: "crossed"
            PropertyChanges { target: label; text: "" }
            PropertyChanges { target: circle; border.width: 0; color: "#dadada" }
            PropertyChanges { target: cross; opacity: 1 }
        }
    ]
}

