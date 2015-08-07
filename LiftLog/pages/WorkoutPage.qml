import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQml.Models 2.1
import LiftLog 1.0
import "../components"

BasicPage {
    id: root
    showNavigationBarBackButton: true
    showNavigationBarSettingsButton: true
    navigationBar.onBackClicked: {
        goBack()
    }

    onGoBack: {
        if (!appState.currentWorkoutModel.workoutEntity.forceSave && !appState.currentWorkoutModel.workoutEntity.shouldBeSaved) {
            appState.currentWorkoutModel.deleteWorkoutData()
        }

        pageStack.goBack()
    }

    TopNotification {
        id: topNotification
        anchors.top: parent.top
        anchors.topMargin: 0
    }

    function updateUserWeightAndSystem(userWeight, weightSystem) {
        appState.currentWorkoutModel.workoutEntity.userWeight = userWeight
        appState.currentUser.weightSystem = weightSystem

        // Enable the button back.
        exercises.headerItem.enabled = true
    }

    function checkIfWorkoutShouldBeSaved() {
        // Count how many sets were completed, if none were completed, we don't want
        // to save this workout in the DB.
        var setsCompleted = appState.currentWorkoutModel.getCompletedSetsCountForAllExercises();
        if (setsCompleted > 0) {
            appState.currentWorkoutModel.workoutEntity.shouldBeSaved = true
        } else {
            appState.currentWorkoutModel.workoutEntity.shouldBeSaved = false
        }
    }

    Component.onCompleted: checkIfWorkoutShouldBeSaved()

    VisualDataModel {
        id: exercisesDelegateModel
        model: appState.currentWorkoutModel
        delegate: ExerciseStatsInput {
            exerciseName: model.name
            exerciseModelIndex: exercisesDelegateModel.modelIndex(index)
            exerciseIndex: index
            setsAndReps: model.setsAndReps
            weight: appState.getWeightString(model.weight, model.exerciseEntity.isAccessory())
            currentWeightText: appState.getWeightString(model.weight, model.exerciseEntity.isAccessory())
            nextWeightText: appState.getWeightString(model.weight + model.weightIncrement, model.exerciseEntity.isAccessory())
            onClicked: {
                // Always reset to the standard state, so that in case if all sets were completed,
                // the timer until the completed label appears is always reset.
                setStandardState()

                // Reset all blinking sets.
                appState.currentWorkoutModel.resetBlinkingSets()

                // Check if workout should be saved based on number of attempted sets.
                checkIfWorkoutShouldBeSaved()

                // Display completed state.
                if (setsCompleted) {
                    setCompletedState()
                    topNotification.hide(true)
                }
                // Hide notification if no reps are done.
                else if (reps == -1) {
                    topNotification.hide(true)
                // Display notification.
                } else if (reps <= repsToDo) {
                    topNotification.hide(true)
                    topNotification.start()
                }
            }
            Component.onCompleted: {
                checkIfCompletedAndSuccesful(exerciseIndex)
                if (setsCompleted) {
                    setCompletedState(true)
                }
            }
        }
    }

    ListView {
        id: exercises
        width: root.width
        spacing: 7 * units.scale
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        anchors.top: parent.top
        anchors.topMargin: 7 * units.scale

        boundsBehavior: Flickable.StopAtBounds
        model: exercisesDelegateModel
        clip: true
        orientation: Qt.Vertical
        header: DateAndWeight {
            id: dateAndWeight
            workoutDate: appState.currentWorkoutModel.workoutEntity.dateStarted
            weightText: appState.getWeightString(appState.currentWorkoutModel.workoutEntity.userWeight, User.Metric, appState.currentUser.weightSystem)

            onBodyWeightClicked: {
                // Disable button so you don't accidentally double click.
                dateAndWeight.enabled = false

                // Show body weight page.
                pageStack.showBodyWeightPage(appState.currentWorkoutModel.workoutEntity.userWeight, appState.currentUser.weightSystem)
            }
        }
        footer: FinishWorkoutButton {
            id: bottomButton
            Binding {
                when: appState.currentWorkoutModel.workoutEntity.completed
                property: "text"
                target: bottomButton
                value: qsTr("UPDATE WORKOUT")
            }

            onClicked: {
                topNotification.hide(true)
                // Explicitly save, even if empty.
                appState.currentWorkoutModel.workoutEntity.shouldBeSaved = true
                appState.currentWorkoutModel.saveWorkoutData()
                goBack()
            }
        }
    }

    modalPopup.onAcceptClicked: {
        console.log("B")
        if (modalPopup.operation === modalPopup.popupDeleteOperation) {
            appState.currentWorkoutModel.deleteWorkoutData()
            goBack()
        }
    }

}
