import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import LiftLog 1.0
import "../components"

Item {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight

    default property alias content: innerItem.data

    Rectangle {
        id: background
        color: "#000000"
        anchors.fill: parent

        Item {
            id: innerItem
            anchors.fill: parent
        }
    }
}
