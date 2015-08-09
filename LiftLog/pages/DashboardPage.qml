import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import LiftLog 1.0
import "../components"

BasicPage {
    id: root

    FontLoader {
        id: icomoon
        source: "qrc:/assets/fonts/icomoon.ttf"
    }

    function buttonStageChange(state) {
        if (state === "pressed") layout.enabled = false
        else layout.enabled = true
    }

    GridLayout {
        id: layout

        anchors.left: parent.left
        anchors.leftMargin: 20 * units.scale
        anchors.right: parent.right
        anchors.rightMargin: 20 * units.scale

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10 * units.scale
        anchors.top: parent.top
        anchors.topMargin: 20 * units.scale

        rowSpacing: 10 * units.scale
        columnSpacing: 10 * units.scale
        columns: 2
        rows: 3

        property int buttonWidth: width * 0.4

        RectangleButton {
            id: newWorkoutButton
            iconText: "\uEA0A"
            labelText: "New workout"
            Layout.fillWidth: true
            Layout.fillHeight: true
            onClicked: {
                pageStack.showWorkoutPage()
            }
            onStateChanged: buttonStageChange(state)
        }

        RectangleButton {
            id: historyButton
            iconText: "\ue953"
            labelText: "History"
            Layout.fillWidth: true
            Layout.fillHeight: true
            onClicked: pageStack.showCalendarPage()
            onStateChanged: buttonStageChange(state)
        }

        RectangleButton {
            id: graphButton
            iconText: "\ue610"
            labelText: "Graph"
            Layout.fillWidth: true
            Layout.fillHeight: true
            onClicked: {
                showModalPopup()
            }
            onStateChanged: buttonStageChange(state)
        }

        RectangleButton {
            id: settingsButton
            iconText: "\ue994"
            labelText: "Settings"
            Layout.fillWidth: true
            Layout.fillHeight: true
            onClicked: {
                pageStack.showBottomPopupView()
            }

            onStateChanged: buttonStageChange(state)
        }

        RectangleButton {
            iconText: ""
            labelText: "Remove DB"
            Layout.fillWidth: true
            Layout.fillHeight: true
            onClicked: {
                dbManager.deleteDBFileAndReInit()
                pageStack.showWelcomePage()
            }
            onStateChanged: buttonStageChange(state)
        }

        //            Label {
        //                text: qsTr("Welcome back, %1").arg(appState.currentUser.name)
        //                opacity: appState.getActiveUserId() !== 0 ? 1 : 0
        //            }
        Connections {
            target: LocalNotificationService
            onNotificationFired: {
                console.log("Notification signal emitted with id " + notificationId)
            }
        }

        Connections {
            target: graphButton
            onClicked: {
                LocalNotificationService.cancelNotification("id5")
            }
            onStateChanged: buttonStageChange(state)
        }

        RectangleButton {
            iconText: "\uea0c"
            labelText: "Help"
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height / 4
            Layout.columnSpan: 2
            onStateChanged: buttonStageChange(state)
        }
    }
}
