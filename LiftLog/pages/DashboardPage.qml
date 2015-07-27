import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import LiftLog 1.0
import "../components"

Item {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight

    Rectangle {
        id: background
        color: "#ecf0f1"
        anchors.fill: parent

        NavigationBar {
            id: navigation_bar
            anchors.top: parent.top
            anchors.topMargin: 0
            width: parent.width
        }

        FontLoader {
            id: icomoon
            source: "qrc:/assets/fonts/icomoon.ttf"
        }

        GridLayout {
            id: gridLayout1
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10

            anchors.top: navigation_bar.bottom
            anchors.topMargin: 20
            rowSpacing: 10
            columnSpacing: 10
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
            }

            RectangleButton {
                id: historyButton
                iconText: "\ue94e"
                labelText: "History"
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: Qt.quit()
            }

            RectangleButton {
                id: graphButton
                iconText: "\ue99b"
                labelText: "Graph"
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            RectangleButton {
                id: settingsButton
                iconText: "\ue994"
                labelText: "Settings"
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

//            RectangleButton {
//                iconText: ""
//                labelText: "Remove DB"
//                Layout.fillWidth: true
//                Layout.fillHeight: true
//                onClicked: {
//                    dbManager.deleteDBFileAndReInit()
//                    pageStack.showWelcomePage()
//                }
//            }

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
            }

            RectangleButton {
                iconText: "\uea0c"
                labelText: "Help"
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height / 4
                Layout.columnSpan: 2
            }
        }
    }
}
