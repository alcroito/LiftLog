import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    id: root
    width: appState.windowWidth
    height: 42 * units.scale

    property date workoutDate: new Date()
    property string dateFormat: "ddd MMM d"
    property alias dateText: dateLabel.text
    property alias weightText: weightLabel.text

    signal dateClicked
    signal bodyWeightClicked

    Rectangle {
        id: inner
        color: "white"
        anchors.rightMargin: 7 * units.scale
        anchors.leftMargin: 7 * units.scale
        anchors.bottomMargin: 7 * units.scale
        anchors.fill: parent

        RowLayout {
            id: layout
            spacing: 0
            anchors.fill: parent

            Rectangle {
                id: date
                Layout.minimumWidth: inner.width / 2
                Layout.minimumHeight: inner.height
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#e1352d"

                Label {
                    id: dateLabel
                    text: "Mon Jun 20"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#ffffff"
                    Component.onCompleted: {
                        text = Qt.binding(function() { return Qt.formatDateTime(workoutDate, dateFormat); })
                    }
                }

                MouseArea {
                    anchors.fill: date
                    onClicked: dateClicked()
                }
            }

            Rectangle {
                id: weight
                Layout.minimumWidth: inner.width / 2
                Layout.minimumHeight: inner.height
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#ffffff"

                Label {
                    id: weightLabel
                    text: "50KG"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#000000"
                }

                MouseArea {
                    anchors.fill: weight
                    onClicked: bodyWeightClicked()
                }
            }
        }
    }
}

