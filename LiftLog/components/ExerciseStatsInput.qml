import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQml.Models 2.1
import LiftLog 1.0

Item {
    id: root
    width: appState.windowWidth
    height: 95 * units.scale
    signal clicked(int columnId, int reps, int repsToDo, bool setsCompleted)
    property alias exerciseName: exerciseNameLabel.text
    property alias setsAndReps: setsAndRepsLabel.text
    property alias weight: weightLabel.text
    property var exerciseModelIndex
    property int exerciseIndex: -1
    property bool setsCompleted: false
    property string nextWeightText: "20KG"
    property string currentWeightText: "20KG"
    property string successText: qsTr("Congrats! %1 next time")
    property string failureText: qsTr("Try reapeating %1 next time, or deload!")
    property bool exerciseIsSuccessful: true

    state: "standard"

    Rectangle {
        id: inner
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
                type: "standard"
            }
            ListElement {
                type: "active"
            }
            ListElement {
                type: "crossed"
            }
            ListElement {
                type: "blinking"
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
                text: model.repsDoneCount
                repsDone: model.repsDoneCount
                repsToDo: model.repsToDoCount
                setModelIndex: setsDelegateModel.modelIndex(index)
                onClicked: {
                    // Modifies the setsCompleted flag.
                    checkIfAllSetsCompleted(root.exerciseIndex)

                    // Modifies the exerciseIsSuccessful flag.
                    checkIfExerciseIsSuccessful(root.exerciseIndex)

                    // Signal parent about the pressed set and if all sets are completed.
                    root.clicked(index, repsDone, repsToDo, setsCompleted)
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
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 4 * units.scale
            anchors.top: topRow.bottom
            anchors.topMargin: 6 * units.scale

            spacing: 15 * units.scale
            contentWidth: parent.width
            boundsBehavior: Flickable.StopAtBounds

            model: setsDelegateModel
            clip: true
            orientation: Qt.Horizontal
            opacity: 1
        }

        Rectangle {
            id: setsCompletedContainer
            opacity: 0

            height: 55 * units.scale
            anchors.right: parent.right
            anchors.rightMargin: 8 * units.scale
            anchors.left: parent.left
            anchors.leftMargin: 8 * units.scale
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 4 * units.scale
            anchors.top: topRow.bottom
            anchors.topMargin: 6 * units.scale

            Label {
                id: setsCompletedLabel
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                Binding on text {
                    value: successText.arg(nextWeightText)
                    when: exerciseIsSuccessful
                }

                Binding on text {
                    value: failureText.arg(currentWeightText)
                    when: !exerciseIsSuccessful
                }
            }

            MouseArea {
                id: completedMouseArea
                anchors.fill: parent
                enabled: false
                onClicked: {
                    root.state = "standard"
                }
            }
        }
    }

    states: [
        State {
            name: "standard"
        },
        State {
            name: "completed"
            PropertyChanges {
                target: circles
                opacity: 0
            }
            PropertyChanges {
                target: setsCompletedContainer
                opacity: 1
            }
            PropertyChanges {
                target: completedMouseArea
                enabled: true
            }
        }
    ]

    transitions: [
        Transition {
            from: "standard"
            to: "completed"
            SequentialAnimation {
                id: completionAnimation
                // Make sure there is a small pause before showing the completed state of the exercise.
                PauseAnimation {
                    duration: 1300
                }
                // The duration 0 property animations are used so that the opacity changes defined in the state
                // apply after the pause animation.
                PropertyAnimation {
                    target: circles
                    property: "opacity"
                    duration: 0
                }
                PropertyAnimation {
                    target: setsCompletedContainer
                    property: "opacity"
                    duration: 0
                }
                PropertyAnimation {
                    target: completedMouseArea
                    property: "enabled"
                    duration: 0
                }
            }
        },
        Transition {
            from: "completed"
            to: "standard"
            SequentialAnimation {
            }
        }
    ]

    function getCompletedSetsCount(exerciseIndex) {
        var completedExercisesCount = 0
        var theModel = appState.currentWorkoutModel
        var setsCount = theModel.setsCountForExercise(exerciseIndex)
        for (var setIndex = 0; setIndex < setsCount; setIndex++) {
            var setAndRep = theModel.getSet(exerciseIndex, setIndex)
            if (setAndRep.isCompleted())
                completedExercisesCount++
        }
        return completedExercisesCount
    }

    function checkIfAllSetsCompleted(exerciseIndex) {
        var theModel = appState.currentWorkoutModel
        var setsCount = theModel.setsCountForExercise(exerciseIndex)
        setsCompleted = root.getCompletedSetsCount(exerciseIndex) === setsCount
    }

    function checkIfExerciseIsSuccessful(exerciseIndex) {
        exerciseIsSuccessful = true
        var theModel = appState.currentWorkoutModel
        var setsCount = theModel.setsCountForExercise(exerciseIndex)
        for (var setIndex = 0; setIndex < setsCount; setIndex++) {
            var setAndRep = theModel.getSet(exerciseIndex, setIndex)
            var success = setAndRep.isSuccessful()
            if (!success) {
                exerciseIsSuccessful = false
                break
            }
        }
    }
}
