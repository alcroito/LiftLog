import QtQuick 2.0
import QtQml 2.2
import LiftLog 1.0

Rectangle {
    id: root
    width: appState.windowWidth
    height: 64 * units.scale
    color: "#2f2f2f"

    property alias text: label.text
    property string successText: qsTr("Well done for doing %1 reps.\n If it was easy, rest for 90 secs. If not, 3 min.", "0", repsDone).arg(repsDone)
    property int repsDone: 0
    property string failureText: "Don't worry, failing is part of the game.\n Rest for 5 mins."
    property bool successState: true

    property alias bottomText: timerText.text
    property date templateElapsedTime: new Date(2015, 0, 1, 0, 0, 0, 0)
    property date startTimerTime: templateElapsedTime
    property date elapsedTime: templateElapsedTime
    property string timerFormat: "mm:ss"

    z: 2
    state: "hidden"

    MouseArea {
        id: mouseBlockerForElementsBehindThisOne
        propagateComposedEvents: false
        anchors.fill: parent
        enabled: true
    }

    function start() {
        state = ""
        reset()
        timer.start()

        // Schedule local notifications to OS.
        var notificationTime = startTimerTime

        notificationTime.setSeconds(notificationTime.getSeconds() + 91);
        LocalNotificationService.scheduleNotification("first", notificationTime, "first", "first", "first", 0)

        notificationTime.setSeconds(notificationTime.getSeconds() + 180);
        LocalNotificationService.scheduleNotification("second", notificationTime, "second", "second", "second", 0)

        notificationTime.setSeconds(notificationTime.getSeconds() + 300);
        LocalNotificationService.scheduleNotification("third", notificationTime, "third", "third", "third", 0)
    }

    function hide(immediate) {
        if (immediate === true) {
            root.opacity = 0
        }
        state = "hidden"
    }

    function reset() {
        LocalNotificationService.cancelAllNotifications()

        startTimerTime = LocalNotificationService.getCurrentDateTime()
        elapsedTime = templateElapsedTime
        timerText.text = Qt.formatDateTime(elapsedTime, timerFormat)

    }

    states: [
        State {
            name: "hidden"
            PropertyChanges { target: root; anchors.topMargin: -root.height }
            PropertyChanges { target: timer; running: false; explicit: true }
            PropertyChanges { target: mouseBlockerForElementsBehindThisOne; enabled: false }
        }
    ]

    transitions: [
        Transition {
            from: ""; to: "hidden"
            SequentialAnimation {
                ScriptAction { script: reset() }
                PropertyAnimation { properties: "anchors.topMargin"; duration: 100; }
                PropertyAction { target: root; property: "opacity"; value: 0; }
            }
        },
        Transition {
            from: "*"; to: ""
            SequentialAnimation {
                // Make sure there is a small pause before showing the notification.
                PropertyAction { target: root; property: "opacity"; value: 0; }
                PauseAnimation { duration: 1000; }
                PropertyAction { target: root; property: "opacity"; value: 1; }
            }
        }
    ]

    Text {
        id: label
        color: "#ffffff"
        text: {
            if (successState) return successText
            else return failureText
        }

        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 4 * units.scale
        font.pixelSize: 8 * units.fontScale
    }

    Timer {
        id: timer
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            var currentTime = LocalNotificationService.getCurrentDateTime()
            var deltaMilliseconds = currentTime - startTimerTime

            var finalTime = templateElapsedTime
            finalTime.setSeconds(Math.floor(deltaMilliseconds / 1000))
            elapsedTime = finalTime

            timerText.text = Qt.formatDateTime(elapsedTime, timerFormat)
        }
    }

    Text {
        id: timerText
        color: "#ffffff"
        text: "00:00"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5 * units.scale
        anchors.horizontalCenter: parent.horizontalCenter
        font.bold: true
        font.pixelSize: 11 * units.fontScale
    }

    Item {
        id: crossArea
        width: cross.width + 10 * units.scale
        height: cross.height + 10 * units.scale

        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        MouseArea {
            anchors.fill: crossArea
            onClicked: hide(false)
        }

        FontLoader {
            id: icomoon
            source: "qrc:/assets/fonts/icomoon.ttf"
        }

        Text {
            id: cross
            color: "#ffffff"
            text: "\uE604"
            font.family: icomoon.name
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            font.pixelSize: 14 * units.fontScale
        }
    }
}
