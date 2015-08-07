import QtQuick 2.0
import QtQuick.Controls 1.2

Item {
    id: root
    width: appState.windowWidth
    height: 54 * units.scale
    property alias text: textLabel.text
    signal clicked

    states: [
        State {
            name: "pressed"
            PropertyChanges {
                target: pressedShadow; opacity: 0.2
            }
        }
    ]
    transitions: [
        Transition {
            from: ""; to: "pressed"
            SequentialAnimation {
                PauseAnimation { duration: 500 }
                PropertyAction { target: root; property: "state"; value: "" }
            }
        }
    ]

    Rectangle {
        id : inner
        color: "#e1352d"
        anchors.fill: parent
        anchors.topMargin: 7 * units.scale
        anchors.bottomMargin: 7 * units.scale
        anchors.leftMargin: 7 * units.scale
        anchors.rightMargin: 7 * units.scale

        Rectangle {
            id: pressedShadow
            anchors.fill: parent
            color: "#000000"
            opacity: 0
        }

        Label {
            id: textLabel
            color: "#ffffff"
            text: qsTr("FINISH WORKOUT")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 10 * units.fontScale
            z: 1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                root.state = "pressed"
                root.clicked()
            }
        }
    }
}
