import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQml.Models 2.1
import LiftLog 1.0
import "../components"

BasicPage {
    id: root
    showNavigationBarBackButton: true
    showNavigationBarBurgerButton: true
    showNavigationBarWarmupButton: true
    navigationBar.onBackClicked: {
        goBack()
    }

    onGoBack: {
        // If the model needs to be saved, don't delete the workout data when going back a page.
        if (!appState.currentWorkoutModel.workoutEntity.forceSave && !appState.currentWorkoutModel.workoutEntity.shouldBeSaved) {
            appState.currentWorkoutModel.deleteWorkoutData()
        // In case if the workout is not completed yet, but we need to save it because some stats data was specified, make sure we save the data
        // but don't mark the entity as completed yet.
        } else if (!appState.currentWorkoutModel.workoutEntity.completed) {
            appState.currentWorkoutModel.saveWorkoutData(false)
        }

        pageStack.goBack()
    }

    function updateUserWeightAndSystem(userWeight, weightSystem) {
        appState.currentWorkoutModel.workoutEntity.userWeight = userWeight
        appState.currentUser.weightSystem = weightSystem

        // Enable the DateAndWeight button back.
        exercises.headerItem.enabled = true
    }

    function updateExerciseWeight(exerciseWeight, setAndRepId, setsAndRepsString, exerciseIndex) {
        // Set the exercise weight.
        appState.currentWorkoutModel.setData(appState.currentWorkoutModel.getExerciseModelIndex(exerciseIndex), exerciseWeight, WorkoutModel.ExerciseWeightRole)

        // Set the set and rep id.
        appState.currentWorkoutModel.setData(appState.currentWorkoutModel.getExerciseModelIndex(exerciseIndex), setAndRepId, WorkoutModel.ExerciseSetAndRepIdRole)

        // Set the set and rep string.
        appState.currentWorkoutModel.setData(appState.currentWorkoutModel.getExerciseModelIndex(exerciseIndex), setsAndRepsString, WorkoutModel.ExerciseSetsAndRepsRole)

        // Enable the weight button back.
        exercises.getDelegateInstanceAt(exerciseIndex).enableExerciseWeightClickArea()

        // Re-initialize exercise sets.
        appState.currentWorkoutModel.reInitializeExerciseSets()

        // Scroll to blinking set.
        scrollToBlinkingSetIfAny()

        // Hide any notifications.
        topNotification.hide(true)
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

    function scrollToBlinkingSetIfAny() {
        var pair = appState.currentWorkoutModel.getBlinkingSetId()
        if (pair.exerciseIndex !== -1 && pair.setIndex !== -1) {
            exercises.positionViewAtIndex(pair.exerciseIndex, ListView.Center)
            exercises.getDelegateInstanceAt(pair.exerciseIndex).circles.positionViewAtIndex(pair.setIndex, ListView.Center)
        }
    }

    Component.onCompleted: checkIfWorkoutShouldBeSaved()

    TopNotification {
        id: topNotification
        anchors.top: parent.top
        anchors.topMargin: 0
    }

    VisualDataModel {
        id: exercisesDelegateModel
        model: appState.currentWorkoutModel
        delegate: ExerciseStatsInput {
            // Used to find the delegate by index in the list view.
            objectName: "exerciseStatsInputDelegate"
            property int objectIndex: index

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
            onWeightClicked: {
                // Disable button so you can't accidentally double click.
                disableExerciseWeightClickArea()

                // Show exercise weight page.
                pageStack.showExerciseWeightPage(
                            model.weight, model.exerciseEntity.getIdSetAndRep(),
                            appState.currentUser.weightSystem, model.weightIncrement, model.exerciseEntity.exerciseCategory(),
                            model.exerciseEntity, index)
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

            onDateClicked: {
                showDatePicker()
            }

            onBodyWeightClicked: {
                // Disable button so you can't accidentally double click.
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
                appState.currentWorkoutModel.workoutEntity.completed = true
                goBack()
            }
        }

        function getDelegateInstanceAt(index) {
            for(var i = 0; i < contentItem.children.length; ++i) {
                var item = contentItem.children[i];
                // We have to check for the specific objectName we gave our
                // delegates above, since we also get some items that are not
                // our delegates here.
                if (item.objectName === "exerciseStatsInputDelegate" && item.objectIndex === index)
                    return item;
            }
            return undefined;
        }
    }

    Connections {
        target: modalPopup
        onAcceptClicked: {
            if (modalPopup.operation === modalPopup.popupDeleteOperation) {
                appState.currentWorkoutModel.deleteWorkoutData()
                goBack()
            }
        }
    }

    Binding {
        target: datePickerDialogLoader
        property: "initialDate"
        value: appState.currentWorkoutModel.workoutEntity.dateStarted
    }

    Connections {
        target: datePickerDialog
        onAcceptClicked: {
            appState.currentWorkoutModel.changeAndSaveStartDate(datePickerDialog.getSelectedDate())
        }
    }
}
