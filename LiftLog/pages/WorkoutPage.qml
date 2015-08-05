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
    navigationBar.onBackClicked: goBack()

    TopNotification {
        id: topNotification
        anchors.top: parent.top
        anchors.topMargin: 0
    }

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
                state = "standard"

                // Reset all blinking sets.
                appState.currentWorkoutModel.resetBlinkingSets()

                // Display completed state.
                if (setsCompleted) {
                    state = "completed"
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
            weightText: appState.getWeightString(appState.currentWorkoutModel.workoutEntity.userWeight)
        }
        footer: FinishWorkoutButton {
            onClicked: {
                appState.currentWorkoutModel.saveWorkoutData()
                goBack()
            }
        }
    }

    SwipeArea {
        id: settingsSwipeArea
        onSwipeLeft: showSettings()

        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
    }
}
