import QtQuick 2.0

FocusScope {
    id: item1
    width: 200
    height: textInput.contentHeight + (2 * textInput.anchors.topMargin);

    Component.onCompleted: print(x, y, width, height)

    property alias text: textInput.text

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#ffffff"
        border.color: "#000000"
    }

    TextInput {
        id: textInput
        focus: true
        text: "Hello world"

        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.bottomMargin: 5
        anchors.topMargin: 5
        font.pointSize: units.sp(12)
    }
}



