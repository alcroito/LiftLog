import QtQuick 2.4
import QtQuick.Controls 1.3
//import QtGraphicalEffects 1.0

Item {
    id: container
    property alias iconText: icon.text
    property alias labelText: label.text
    property alias buttonWidth: container.width
    signal clicked()

    width: 100
    height: 100

    states: [
        State {
            name: "pressed"
            PropertyChanges {
                target: pressedShadow; opacity: 0.1
                enabled: false
            }
        }
    ]
    transitions: [
        Transition {
            from: ""; to: "pressed"
            SequentialAnimation {
                PauseAnimation { duration: 500 }
                PropertyAction { target: container; property: "state"; value: "" }
            }
        }
    ]

    Rectangle {
        id: rectangle
        anchors.fill: parent
        border.width: 1
        border.color: "#e4dfdf"
    }

    Rectangle {
        id: pressedShadow
        anchors.fill: parent
        color: "#000000"
        opacity: 0
    }

    MouseArea {
       id: mouseArea
       anchors.fill: parent
       onClicked: {
           container.state = "pressed"
           container.clicked()
       }
    }

//    DropShadow {
//        anchors.fill: rectangle
//        horizontalOffset: 1
//        verticalOffset: 1
//        radius: 8.0
//        samples: 16
//        color: "#80000000"
//        source: rectangle
//        spread: 0.5
//        fast: true
    //     }

//    Shadow {
//        anchors.fill: rectangle
//        blur: 10
//        color: "#000000"

//        anchors.rightMargin: 10
//        anchors.leftMargin: 10
//        anchors.bottomMargin: 10
//        anchors.topMargin: 10
//        z: -1
//        horizontalOffset: 1
//        verticalOffset: 1
//    }

    FontLoader {
        id: icomoon
        source: "qrc:/assets/fonts/icomoon.ttf"
    }

    Text {
        id: icon
        font.family: icomoon.name
        text: qsTr("\uEA0A")
        font.pointSize: 18
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Text {
        id: label
        color: "#e74c3c"
        text: "New Workout"
        font.pixelSize: units.sp(10)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7
    }
}

