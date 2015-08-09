import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.3
import LiftLog 1.0
import "../components"

Item {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight

    property color backgroundColor: "white"
    property color barColor: "#f7f7f7"
    property color textColor: "#e74c3c"
    property alias acceptButtonText: acceptButton.text
    property alias rejectButtonText: rejectButton.text
    default property alias content: popupContent.data
    property alias popupBackground: popupBackground

    signal acceptClicked
    signal rejectClicked
    signal anyClicked

    Rectangle {
        id: background
        color: "#000000"
        anchors.fill: parent
        opacity: 0.5
    }

    Rectangle {
        id: popupBackground
        color: backgroundColor
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -height
        width: root.width
        height: 208 * units.scale

        Column {
            id: layout

            Rectangle {
                id: row
                width: root.width
                height: childrenRect.height
                color: barColor

                Label {
                    id: rejectButton
                    height: 50 * units.scale
                    width: popupBackground.width / 2
                    text: qsTr("Cancel")
                    anchors.left: parent.left
                    anchors.leftMargin: 10 * units.scale
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    color: textColor
                    font.pixelSize: 10 * units.fontScale

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.rejectClicked()
                            root.anyClicked()
                        }
                    }
                }

                Label {
                    id: acceptButton
                    height: 50 * units.scale
                    width: popupBackground.width / 2
                    text: qsTr("Set")
                    anchors.right: parent.right
                    anchors.rightMargin: 10 * units.scale
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    color: textColor
                    font.pixelSize: 10 * units.fontScale

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.acceptClicked()
                            root.anyClicked()
                        }
                    }
                }
            }
        }

        Item {
            id: popupContent
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.top: layout.bottom
            anchors.topMargin: 0
        }
    }
}
