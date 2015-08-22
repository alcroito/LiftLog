import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import LiftLog 1.0
import "../components"

BasicPage {
    id: root
    showNavigationBarBackButton: true
    navigationBar.onBackClicked: {
        goBack()
    }

    rootBackground.color: showGraph ? "white" : "#ecf0f1"
    width: appState.windowWidth
    height: appState.windowHeight

    property bool showGraph: false

    Label {
        id: progressLabel
        text: qsTr("All Progress")
        anchors.verticalCenter: progressSelectorBackground.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 8 * units.scale
        font.pixelSize: 12 * units.fontScale
        visible: showGraph
        enabled: showGraph
    }

    Rectangle {
        id: progressSelectorBackground
        color: "#e1352d"
        radius: 20 * units.scale
        anchors.right: parent.right
        anchors.rightMargin: 8 * units.scale
        anchors.top: parent.top
        anchors.topMargin: 9 * units.scale
        height: 40 * units.scale
        width: 85 * units.scale
        visible: showGraph
        enabled: showGraph

        ComboBox {
            id: progressSelectorComboBox
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            width: progressSelectorBackground.width
            height: progressSelectorBackground.height
            model: ListModel {
                ListElement { text: qsTr("All") }
                ListElement { text: qsTr("6 months") }
                ListElement { text: qsTr("3 monhts") }
                ListElement { text: qsTr("30 days") }
            }
            style: ComboBoxStyle {
                background: Item {
                    anchors.fill: parent
                }
                label: Label {
                    id: progressSelectorLabel
                    color: "white"
                    text: control.currentText
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    font.pixelSize: 8 * units.fontScale
                }
            }
        }
    }

    Component.onCompleted: {
        var count = StatsGraphDataSingleton.getMinPointCountForAnyExerciseFromDB();
        if (count >= 2) {
            showGraph = true;
        }
    }

    Loader {
        id: graphLoader
        active: showGraph
        anchors.top: progressSelectorBackground.bottom
        anchors.topMargin: 10 * units.scale

        sourceComponent: GraphSwipeable {
            id: graph
        }
    }

    Loader {
        id: comeBackLoader
        anchors.topMargin: 10 * units.scale
        anchors.right: parent.right
        anchors.rightMargin: 10 * units.scale
        anchors.left: parent.left
        anchors.leftMargin: 10 * units.scale
        anchors.verticalCenter: parent.verticalCenter

        active: !showGraph
        sourceComponent: Rectangle {
            color: "white"
            height: childrenRect.height + 40 * units.scale
            Label {
                id: comeBackLabel
                text: qsTr("You have no workouts done yet.\n\nCome back to this page after you've done a few.")
                font.pixelSize: 10 * units.fontScale
                wrapMode: Text.Wrap
                anchors.top: parent.top
                anchors.topMargin: 20 * units.scale
                anchors.left: parent.left
                anchors.leftMargin: 10 * units.scale
                anchors.right: parent.right
                anchors.rightMargin: 10 * units.scale
            }
        }
    }
}
