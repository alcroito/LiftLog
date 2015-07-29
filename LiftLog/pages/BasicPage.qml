import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import LiftLog 1.0
import "../components"

Item {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight

    property Component pageComponent
    property alias navigationBar: navigationBar
    property bool showNavigationBar: true
    property bool showNavigationBarBackButton: false
    default property alias content: innerItem.data

    Rectangle {
        id: background
        color: "#ecf0f1"
        anchors.fill: parent

        NavigationBar {
            id: navigationBar
            anchors.top: parent.top
            anchors.topMargin: 0
            width: parent.width
            opacity: showNavigationBar ? 1 : 0
            showBackButton: showNavigationBarBackButton
        }

        Item {
            id: innerItem
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.top: navigationBar.bottom
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
        }
    }
}
