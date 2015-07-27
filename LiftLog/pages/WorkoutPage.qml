import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQml.Models 2.1
import LiftLog 1.0
import "../components"

Item {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight
    signal goBack

    Rectangle {
        id: background
        color: "#ecf0f1"
        anchors.fill: parent

        NavigationBar {
            id: navigation_bar
            transformOrigin: Item.Center
            anchors.top: parent.top
            anchors.topMargin: 0
            width: parent.width
            state: ""
            onBackClicked: goBack()
        }

        TopNotification {
            id: topNotification
            anchors.top: navigation_bar.bottom
            anchors.topMargin: 0
        }

        VisualDataModel {
            id: exercisesDelegateModel
            model: appState.currentWorkoutModel
            delegate: ExerciseStatsInput {
                exerciseName: model.name
                exerciseModelIndex: exercisesDelegateModel.modelIndex(index)
                setsAndReps: model.setsAndReps
                weight: model.weight + "KG"
                onClicked: {
                    if (reps == -1) {
                        topNotification.hide(true)
                    }
                    else if (reps <= repsToDo) {
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

            anchors.top: navigation_bar.bottom
            anchors.topMargin: 7 * units.scale

            boundsBehavior: Flickable.StopAtBounds
            model: exercisesDelegateModel
            clip: true
            orientation: Qt.Vertical
            header: DateAndWeight {
                MouseArea {
                    anchors.fill: parent
                    onClicked: topNotification.start()
                }
            }
            footer: FinishWorkoutButton {
                onClicked: goBack()
            }
        }
    }
}
