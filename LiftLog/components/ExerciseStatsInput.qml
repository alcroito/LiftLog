import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQml.Models 2.1
import LiftLog 1.0

Item {
    id: root
    width: appState.windowWidth
    height: 95 * units.scale
    signal clicked(int columnId, int reps, int repsToDo)
    property alias exerciseName: exerciseNameLabel.text
    property alias setsAndReps: setsAndRepsLabel.text
    property alias weight: weightLabel.text
    property var exerciseModelIndex

    Rectangle {
        id : inner
        color: "white"
        anchors.rightMargin: 7 * units.scale
        anchors.leftMargin: 7 * units.scale
        anchors.fill: parent        

        Item {
            id: topRow
            height: 39 * units.scale
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0

            Item {
                id: topRowInner
                height: 39 * units.scale
                anchors.right: parent.right
                anchors.rightMargin: 9 * units.scale
                anchors.left: parent.left
                anchors.leftMargin: 7 * units.scale
                anchors.top: parent.top
                anchors.topMargin: 13 * units.scale

                Label {
                    id: exerciseNameLabel
                    text: "Squat"
                    anchors.left: parent.left
                    anchors.leftMargin: 0
                }

                Label {
                    id: setsAndRepsLabel
                    text: "5x5"
                    anchors.right: weightLabel.left
                    anchors.rightMargin: 2 * units.scale
                }

                Label {
                    id: weightLabel
                    text: "20KG"
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                }
            }
        }

        ListModel {
            id: circlesModel
            ListElement {
                type: "empty"
            }
            ListElement {
                type: "empty"
            }
            ListElement {
                type: "standard"
            }
            ListElement {
                type: "active"
            }
            ListElement {
                type: "standard"
            }
            ListElement {
                type: "standard"
            }
            ListElement {
                type: "standard"
            }
        }

        Rectangle {
            id: separator
            anchors.top: topRow.bottom
            anchors.topMargin: 0
            width: inner.width
            height: 1 * units.scale
            z: 1
            color: "#ebebeb"
        }

        VisualDataModel {
            id: setsDelegateModel
            model: appState.currentWorkoutModel
            rootIndex: exerciseModelIndex
            delegate: SetAndRep {
                state: model.state
                text: ""
                reps: model.repCount
                repsToDo: model.repCount
                onClicked: {
                    root.clicked(index, reps, repsToDo)
                }
            }
        }

        ListView {
            id: circles
            height: 55 * units.scale
            anchors.right: parent.right
            anchors.rightMargin: 8 * units.scale
            anchors.left: parent.left
            anchors.leftMargin: 8 * units.scale
            spacing: 15 * units.scale
            contentWidth: parent.width
            boundsBehavior: Flickable.StopAtBounds
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 4 * units.scale
            anchors.top: topRow.bottom
            anchors.topMargin: 6 * units.scale
            model: setsDelegateModel
            clip: true
            orientation: Qt.Horizontal
        }
    }
}

