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

    onIsGoingToBeShown: {
        appState.recheckUncompletedWorkoutExistsValue()
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
            iconText: !appState.uncompletedWorkoutExists ? "\uEA0A" : "\uE611"
            labelText: !appState.uncompletedWorkoutExists ? qsTr("New workout") : qsTr("Continue workout")
            icon.font.pixelSize: !appState.uncompletedWorkoutExists ? 12 * units.fontScale : 16 * units.fontScale
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
            iconText: "\ue60d"
            labelText: "Graph"
            Layout.fillWidth: true
            Layout.fillHeight: true
            onClicked: {
                pageStack.showGraphPage()
            }
            onStateChanged: buttonStageChange(state)
        }

        RectangleButton {
            id: settingsButton
            iconText: "\ue612"
            labelText: "Settings"
            Layout.fillWidth: true
            Layout.fillHeight: true
            onClicked: {
                pageStack.showSettingsPage()
            }

            onStateChanged: buttonStageChange(state)
        }

        RectangleButton {
            iconText: ""
            labelText: "Remove DB"
            Layout.fillWidth: true
            Layout.fillHeight: true
            onClicked: {
                showModalPopup()
                modalPopup.prepare("deletedb")
            }
            onStateChanged: buttonStageChange(state)
        }

        Connections {
            target: modalPopup
            onAcceptClicked: {
                if (modalPopup.operation === modalPopup.popupDeleteDBOperation) {
                    dbManager.deleteDBFileAndReInit()
                    pageStack.showWelcomePage()
                }
            }
        }


        //            Label {
        //                text: qsTr("Welcome back, %1").arg(appState.currentUser.name)
        //                opacity: appState.getActiveUserId() !== 0 ? 1 : 0
        //            }
        Connections {
            target: LocalNotificationService
            function onNotificationFired(notificationId) {
                console.log("Notification signal emitted with id " + notificationId)
            }
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
