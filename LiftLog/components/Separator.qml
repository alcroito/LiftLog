import QtQuick 2.0

Item {
    id: root
    width: separator.width + separator.anchors.leftMargin + separator.anchors.rightMargin
    height: separator.height + separator.anchors.topMargin + separator.anchors.bottomMargin

    property alias color: separator.color
    property alias separatorWidth: separator.width
    property alias separatorHeight: separator.height
    property alias separator: separator

    Rectangle {
        id: separator
        width: 400
        height: 4
        anchors.fill: parent
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        color: "orange"
    }
}

