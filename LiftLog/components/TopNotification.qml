import QtQuick 2.0
import QtQml 2.2

Rectangle {
    id: root
    width: appState.windowWidth
    height: 64 * units.scale
    color: "#2f2f2f"
    property alias text: label.text
    property alias bottomText: timerText.text
    property date elapsedTime: new Date(2015, 1, 2, 0, 0, 0, 0)
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
    }

    function hide(immediate) {
        if (immediate === true) {
            root.opacity = 0
        }
        state = "hidden"
    }

    function reset() {
        elapsedTime = new Date(2015, 1, 2, 0, 0, 0, 0)
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
        text: "Well done for doing x reps.\n Rest for 90 sec."
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
            var tempDate = elapsedTime
            tempDate.setSeconds(tempDate.getSeconds() + 1)
            elapsedTime = tempDate
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
