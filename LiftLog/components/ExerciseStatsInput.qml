import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQml 2.15
import QtQml.Models 2.1
import LiftLog 1.0

Item {
    id: root
    width: appState.windowWidth
    height: 95 * units.scale

    property alias exerciseName: exerciseNameLabel.text
    property alias setsAndReps: setsAndRepsLabel.text
    property alias weight: weightLabel.text
    property alias circles: circles
    property var exerciseModelIndex
    property int exerciseIndex: -1
    property bool setsCompleted: false
    property string nextWeightText: "20KG"
    property string currentWeightText: "20KG"
    property string successText: qsTr("Congrats! %1 next time")
    property string failureText: qsTr("Try reapeating %1 next time, or deload!")
    property bool immediateCompletedTransition: false
    property bool exerciseIsSuccessful: true

    signal clicked(int columnId, int reps, int repsToDo, bool setsCompleted)
    signal weightClicked

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

                MouseArea {
                    id: setsAndWeightCointaner
                    width: (setsAndRepsLabel.width + weightLabel.width) + 10 * units.scale
                    height: (setsAndRepsLabel.height + weightLabel.height) + 4 * units.scale
                    anchors.top: parent.top
                    anchors.topMargin: -10 * units.scale

                    anchors.right: parent.right
                    anchors.rightMargin: 0

                    onClicked: {
                        root.weightClicked()
                    }
                    Label {
                        id: setsAndRepsLabel
                        text: "5x5"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: weightLabel.left
                        anchors.rightMargin: 2 * units.scale
                    }

                    Label {
                        id: weightLabel
                        text: "20KG"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                    }
                }
            }
        }

        // Just for testing purposes in Qt Quick Designer.
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
                isUsedForWorkout: true
                onClicked: {
                    checkIfCompletedAndSuccesful(root.exerciseIndex)

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
                    restoreMode: Binding.RestoreBinding
                }

                Binding on text {
                    value: failureText.arg(currentWeightText)
                    when: !exerciseIsSuccessful
                    restoreMode: Binding.RestoreBinding
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
            id: standardToCompletedTransition
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

    Binding {
        target: standardToCompletedTransition
        property: "enabled"
        value: !immediateCompletedTransition
    }

    function getCompletedSetsCount(exerciseIndex) {
        var completedExerciseSetCount = 0
        var theModel = appState.currentWorkoutModel
        var setsCount = theModel.setsCountForExercise(exerciseIndex)
        for (var setIndex = 0; setIndex < setsCount; setIndex++) {
            var setAndRep = theModel.getSet(exerciseIndex, setIndex)
            if (setAndRep.isAttempted())
                completedExerciseSetCount++
        }
        return completedExerciseSetCount
    }

    function checkIfAllSetsCompleted(exerciseIndex) {
        var theModel = appState.currentWorkoutModel
        var setsCount = theModel.setsCountForExercise(exerciseIndex)
        var completedCount = root.getCompletedSetsCount(exerciseIndex)
        setsCompleted = completedCount === setsCount

        appState.currentWorkoutModel.setData(appState.currentWorkoutModel.getExerciseModelIndex(exerciseIndex), setsCompleted, WorkoutModel.ExerciseCompletedAllSetsRole)
        appState.currentWorkoutModel.setData(appState.currentWorkoutModel.getExerciseModelIndex(exerciseIndex), completedCount, WorkoutModel.ExerciseCompletedSetCountRole)
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
        appState.currentWorkoutModel.setData(appState.currentWorkoutModel.getExerciseModelIndex(exerciseIndex), exerciseIsSuccessful, WorkoutModel.ExerciseSuccessfulRole)
    }

    function checkIfCompletedAndSuccesful(index) {
        // Modifies the setsCompleted flag.
        checkIfAllSetsCompleted(index)

        // Modifies the exerciseIsSuccessful flag.
        checkIfExerciseIsSuccessful(index)
    }

    function setStandardState() {
        root.state = "standard"
        immediateCompletedTransition = false
    }

    function setCompletedState(immediate) {
        if (immediate === true) {
            immediateCompletedTransition = true
        }

        root.state = "completed"
    }

    function enableExerciseWeightClickArea() {
        setsAndWeightCointaner.enabled = true
    }

    function disableExerciseWeightClickArea() {
        setsAndWeightCointaner.enabled = false
    }
}
