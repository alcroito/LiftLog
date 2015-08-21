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

    rootBackground.color: "white"
    width: appState.windowWidth
    height: appState.windowHeight

    Label {
        id: progressLabel
        text: qsTr("All Progress")
        anchors.verticalCenter: progressSelectorBackground.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 8 * units.scale
        font.pixelSize: 12 * units.fontScale
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
                    Component.onCompleted: {
                        print("hi", width, height, contentWidth, contentHeight)
                    }
                }
            }
        }

//        MouseArea {
//            id: progressSelectorMouseArea
//            anchors.fill: parent
//            onClicked: {

//            }
//        }
    }

    GraphSwipeable {
        id: graph
        anchors.top: progressSelectorBackground.bottom
        anchors.topMargin: 10 * units.scale
    }
}
