import QtQuick 2.0
import QtQuick.Controls 1.4
import LiftLog 1.0

Item {
    id: sectionContainer
    width: sectionWidth
    height: sectionHeight

    property bool sectionAdditionalItemWillBeShown: false
    property var sectionAdditionalItemComponent: null
    property int sectionHeight: 35 * units.scale
    property int sectionWidth: appState.windowWidth
    property bool centerVerticalSectionItems: false

    property string manualSection: ""

    Label {
        id: sectionLabel
        anchors.left: parent.left
        anchors.leftMargin: 15 * units.scale
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 4 * units.scale
        text: {
            if (manualSection) return manualSection
            if (section) return section
            return ""
        }

        font.pixelSize: 9 * units.fontScale
        color: "#6d6d72"
    }

    Loader {
        id: sectionHeadingAdditionalItem
        anchors.right: parent.right
        anchors.rightMargin: 15 * units.scale
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 4 * units.scale

        sourceComponent: sectionAdditionalItemComponent
        active: sectionAdditionalItemWillBeShown
    }

    state: centerVerticalSectionItems ? "vericallyCentered" : ""

    states: [
        State {
            name: "vericallyCentered"
            AnchorChanges {
                target: sectionLabel
                anchors.bottom: undefined
                anchors.verticalCenter: sectionContainer.verticalCenter
            }
            AnchorChanges {
                target: sectionHeadingAdditionalItem
                anchors.bottom: undefined
                anchors.verticalCenter: sectionContainer.verticalCenter
            }
        }

    ]
}
