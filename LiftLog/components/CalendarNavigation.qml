import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

Item {
    id: root
    width: appState.windowWidth
    height: 75 * units.scale

    property color backgroundColor: "#dfe2e3"
    property date calendarDate: new Date()
    property string dateFormat: "MMMM yyyy"
    signal leftClicked
    signal rightClicked

    FontLoader {
        id: icomoon
        source: "qrc:/assets/fonts/icomoon.ttf"
    }

    Rectangle {
        id: background
        color: backgroundColor
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.rightMargin: 13 * units.scale
        anchors.leftMargin: 13 * units.scale
        anchors.topMargin: 15 * units.scale

        Item {
            id: layout
            anchors.fill: background

            Text {
                id: leftArrow
                font.family: icomoon.name
                font.bold: true
                width: 40 * units.scale
                text: qsTr("\uE605")
                anchors.left: parent.left
                anchors.leftMargin: 10 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 18 * units.fontScale

                MouseArea {
                    anchors.fill: parent
                    onClicked: leftClicked()
                }
            }

            Label {
                id: label
                text: Qt.formatDateTime(calendarDate, dateFormat)
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 10 * units.fontScale
            }

            Text {
                id: rightArrow
                font.family: icomoon.name
                font.bold: true
                width: 40 * units.scale
                text: qsTr("\uE60C")
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 10 * units.scale
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 18 * units.fontScale

                MouseArea {
                    anchors.fill: parent
                    onClicked: rightClicked()
                }
            }
        }
    }
}

