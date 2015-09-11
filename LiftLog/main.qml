import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.0

import LiftLog 1.0
import "components"

ApplicationWindow {
    id: appWindow
    title: qsTr("Lift Log")
    visible: true

    minimumWidth: appState.windowWidth
    minimumHeight: appState.windowHeight
    maximumWidth: appState.windowWidth
    maximumHeight: appState.windowHeight
    width: appState.windowWidth
    height: appState.windowHeight

    property var pageComponents: ({

                                  })

    // Always define a component inline, instead of as a property, so that Qt Quick Designer
    // can properly display the component when it's set as the initial item of a StackView.
    // Also make sure that the "Always use the QML emulation layer provided by Qt Creator"
    // option is disabled in preferences.
    StackView {
        id: pageStack
        anchors.fill: parent
        focus: true

        function initPageStackFirstPage() {
            if (appState.isActiveUserSet()) {
                pageStack.push(pageComponents.dashboardPage)
            } else {
                pageStack.push(pageComponents.welcomePage)
            }
        }

        Component.onCompleted: {

            // Load initial components depending on user active state.
            // Load the necessary component synchronously, and the rest of them asynchronously.
            var dashboardPageMode = Component.PreferSynchronous
            var welcomePageMode = Component.PreferSynchronous

            if (appState.isActiveUserSet()) {
                welcomePageMode = Component.Asynchronous
            } else {
                dashboardPageMode = Component.Asynchronous
            }

            pageComponents.dashboardPage = Qt.createComponent(
                        "pages/DashboardPage.qml", dashboardPageMode, appWindow)
            pageComponents.welcomePage = Qt.createComponent(
                        "pages/WelcomePage.qml", welcomePageMode, appWindow)
            initPageStackFirstPage()

            pageComponents.workoutPage = Qt.createComponent(
                        "pages/WorkoutPage.qml", Component.Asynchronous,
                        appWindow)
            pageComponents.calendarPage = Qt.createComponent(
                        "pages/CalendarPage.qml", Component.Asynchronous,
                        appWindow)
            pageComponents.bodyWeightPage = Qt.createComponent(
                        "pages/BodyWeightPage.qml", Component.Asynchronous,
                        appWindow)
            pageComponents.exerciseWeightPage = Qt.createComponent(
                        "pages/ExerciseWeightPage.qml", Component.Asynchronous,
                        appWindow)
            pageComponents.graphPage = Qt.createComponent(
                        "pages/GraphPage.qml", Component.Asynchronous,
                        appWindow)
            pageComponents.settingsPage = Qt.createComponent(
                        "pages/SettingsPage.qml", Component.Asynchronous,
                        appWindow)
            pageComponents.platesBarbellSettingsPage = Qt.createComponent(
                        "pages/PlatesBarbellSettingsPage.qml", Component.Asynchronous,
                        appWindow)
        }

        delegate: CustomStackViewDelegate {
        }

        Keys.onReleased: {
            if (event.key === Qt.Key_Back && pageStack.depth > 1) {
                currentItem.goBack()
                event.accepted = true
            }
        }

        function createComponentObjectGuarded(component, creationFunction) {
            if (component.status === Component.Ready || component.status === Component.Error)
                finishComponentObjectCreation(component, creationFunction);
            else
                component.statusChanged.connect(function() { finishComponentObjectCreation(component, creationFunction); });
        }

        function finishComponentObjectCreation(component, creationFunction) {
            if (component.status === Component.Ready) {
                var image = creationFunction();
                if (image === null)
                    console.warn("Error creating object based on component.");
            }
            else if (component.status === Component.Error)
                console.warn("Error loading component:", component.errorString());
        }

        function showWelcomePage() {
            createComponentObjectGuarded(pageComponents.welcomePage, function() {
                push({
                         item: pageComponents.welcomePage,
                     }, replace)
            });
        }

        function showDashboardPage() {
            createComponentObjectGuarded(pageComponents.dashboardPage, function() {
                push({
                         item: pageComponents.dashboardPage,
                     }, replace)
            });
        }

        function showWorkoutPage() {
            appState.currentWorkoutModel.clear()
            appState.currentWorkoutModel.getLastNotCompletedWorkoutOrCreateNew()
            appState.currentWorkoutModel.getWorkoutData()

            createComponentObjectGuarded(pageComponents.workoutPage, function() {
                push({
                         item: pageComponents.workoutPage
                     })
            });
        }

        function showSpecificWorkoutPage(date) {
            appState.currentWorkoutModel.clear()
            appState.currentWorkoutModel.getWorkoutOnDateOrCreateNewAtDate(date)
            appState.currentWorkoutModel.getWorkoutData()

            createComponentObjectGuarded(pageComponents.workoutPage, function() {
                push({
                         item: pageComponents.workoutPage
                     })
            });
        }

        function showCalendarPage() {
            createComponentObjectGuarded(pageComponents.calendarPage, function() {
                push({
                         item: pageComponents.calendarPage
                     })
            });
        }

        function showBodyWeightPage(userWeight, weightSystem) {
            var properties = {
                transitionOrientation: "vertical",
                weightSystem: weightSystem,
                weightNumber: userWeight
            }

            createComponentObjectGuarded(pageComponents.bodyWeightPage, function() {
                push({
                         item: pageComponents.bodyWeightPage,
                         properties: properties
                     })
            });
        }

        function showExerciseWeightPage(exerciseWeight, setAndRepId, weightSystem, weightIncrement, exerciseCategory, exerciseEntity, exerciseIndex) {
            var properties = {
                transitionOrientation: "vertical",
                weightNumber: exerciseWeight,
                weightSystem: weightSystem,
                weightIncrement: weightIncrement,
                setAndRepId: setAndRepId,
                exerciseCategory: exerciseCategory,
                exerciseEntity: exerciseEntity,
                exerciseIndex: exerciseIndex
            }

            createComponentObjectGuarded(pageComponents.exerciseWeightPage, function() {
                push({
                         item: pageComponents.exerciseWeightPage,
                         properties: properties
                     })
            });
        }

        function showGraphPage() {
            createComponentObjectGuarded(pageComponents.graphPage, function() {
                push({
                         item: pageComponents.graphPage
                     })
            });

        }

        function showSettingsPage() {
            createComponentObjectGuarded(pageComponents.settingsPage, function() {
                push({
                         item: pageComponents.settingsPage
                     })
            });
        }

        function showSpecificSettingsPage(settingsPageId) {
            if (settingsPageId === "plates") showPlatesBarbellSettingsPage();
            else createComponentObjectGuarded(pageComponents.settingsPage, function() {
                push({
                         item: pageComponents.settingsPage,
                         properties: {
                             settingsPageId: settingsPageId
                         }
                     })
            });
        }

        function showPlatesBarbellSettingsPage() {
            createComponentObjectGuarded(pageComponents.platesBarbellSettingsPage, function() {
                push({
                         item: pageComponents.platesBarbellSettingsPage,
                         properties: {
                         }
                     })
            });
        }

        function goBack() {
            if (pageStack.depth > 1) {
                pageStack.pop()
            }
        }
    }
}
