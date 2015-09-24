import QtQuick 2.4
import QtQuick.Controls 1.2

Loader {
    property bool showDialog: false
    property string dialogText: ""
    property int emptySpaceAbove: 0

    id: comeBackLoader
    anchors.topMargin: 10 * units.scale
    anchors.right: parent.right
    anchors.rightMargin: 10 * units.scale
    anchors.left: parent.left
    anchors.leftMargin: 10 * units.scale
    anchors.verticalCenter: parent.verticalCenter

    active: showDialog
    sourceComponent: Item {
        id: outerContainer
        height: innerContainer.height + emptySpaceAbove

        Rectangle {
            id: innerContainer
            color: "white"
            height: childrenRect.height + 40 * units.scale
            width: parent.width
            anchors.centerIn: parent
            anchors.topMargin: emptySpaceAbove

            Label {
                id: comeBackLabel
                text: dialogText
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
