import QtQuick 2.0

FocusScope {
    id: item1
    width: 200 * units.scale
    height: textInput.contentHeight + (2 * textInput.anchors.topMargin);

    Component.onCompleted: print(x, y, width, height)

    property alias text: textInput.text

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#ffffff"
        border.color: "#000000"
        border.width: 1 * units.scale
    }

    TextInput {
        id: textInput
        focus: true
        text: "Hello world"

        anchors.right: parent.right
        anchors.rightMargin: 5 * units.scale
        anchors.left: parent.left
        anchors.leftMargin: 5 * units.scale
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.bottomMargin: 5 * units.scale
        anchors.topMargin: 5 * units.scale
        font.pixelSize: 12 * units.fontScale
    }
}



