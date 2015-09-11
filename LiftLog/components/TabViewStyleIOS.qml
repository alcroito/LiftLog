import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import LiftLog 1.0

TabViewStyle {
    frameOverlap: 0
    tabOverlap: 0
    tabsAlignment: Qt.AlignHCenter
    tab: Item {
        property real maxTabWidth: control.count > 0 ? styleData.availableWidth / control.count : 0
        implicitWidth: maxTabWidth
        height: 50 * units.scale
        property var tabItem: control.getTab(styleData.index)

        Label {
            id: tabLabel
            text: styleData.title
            Accessible.ignored: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5 * units.scale
            color: styleData.selected ? "#e1352d" : "#929292"
        }

        Label {
            id: tabIcon
            text: tabItem.tabIcon
            font.family: icomoon.name
            font.pixelSize: 16 * units.fontScale
            Accessible.ignored: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 5 * units.scale
            color: styleData.selected ? "#e1352d" : "#929292"
        }
    }

    tabBar: Rectangle {
        Separator {
            id: topBorder
            separatorWidth: parent.width
            separatorHeight: 1 * units.scale
            color: "#a4a4a4"
        }

        width: appState.windowWidth
        height: 50 * units.scale
        color: "#f6f6f6"
    }
}
