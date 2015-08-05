import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.0

import LiftLog 1.0
import LiftLog.pages 1.0
//import LiftLog.utils 1.0
//import "pages"
//import "components"

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

    // Always define a component inline, instead of as a property, so that Qt Quick Designer
    // can properly display the component when it's set as the initial item of a StackView.
    // Also make sure that the "Always use the QML emulation layer provided by Qt Creator"
    // option is disabled in preferences.
    Component {
        id: welcomePage
        WelcomePage {
            Component.onCompleted: console.log("Welcome page loaded")
        }
    }

    Component {
        id: dashboardPage
        DashboardPage {
            Component.onCompleted: console.log("Dashboard page loaded")
        }
    }

    Component {
        id: workoutPage
        WorkoutPage {
            Component.onCompleted: {
                console.log("Workout page loaded")
            }
            onGoBack: pageStack.goBack()
        }
    }

    Component {
        id: calendarPage
        CalendarPage {
            Component.onCompleted: {
                console.log("Calendar page loaded")
            }
            onGoBack: pageStack.goBack()
        }
    }

    StackView {
        id: pageStack
        anchors.fill: parent
        focus: true

        states: [
            State {
                name: "welcome"
                PropertyChanges { target: pageStack; initialItem: welcomePage }
                // Fix to use a property instead of function call, so that QML emulation layer
                // shows the proper page.
                when: !appState.isActiveUserSet()
            },
            State {
                name: "dashboard"
                PropertyChanges { target: pageStack; initialItem: dashboardPage }
                when: appState.isActiveUserSet()
            }
        ]

        Keys.onReleased: {
            if (event.key === Qt.Key_Back && pageStack.depth > 1) {
                 pageStack.goBack()
                 event.accepted = true;
             }
        }

        function showWelcomePage() {
            push({item: welcomePage}, replace)
        }

        function showDashboardPage() {
            push({item: dashboardPage}, replace)
        }

        function showWorkoutPage() {
            appState.currentWorkoutModel.getLastNotCompletedWorkoutOrCreateNew()
            appState.currentWorkoutModel.getWorkoutData()
            push({item: workoutPage})
        }

        function showCalendarPage() {
            push({item: calendarPage})
        }

        function goBack() {
            if (pageStack.depth > 1) {
                pageStack.pop();
            }
        }
    }
}
