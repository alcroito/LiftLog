import QtQuick 2.0
import QtQuick.Controls 1.4
import LiftLog 1.0

CellTypeBase {
    id: cellTypeWrapper

    Loader {
        id: cellIconLoader

        anchors.left: parent.left
        anchors.leftMargin: 15 * units.scale
        anchors.verticalCenter: parent.verticalCenter
        sourceComponent: Label {
            id: cellIcon
            font.family: icomoon.name
            font.pixelSize: 12 * units.fontScale
            text: itemModelData.icon
            Accessible.ignored: true
        }
        active: itemModelData.showIcon
    }

    Label {
        property int leftMarginOfLabel: itemModelData.showIcon ? 40 : 10;
        id: cellLabel
        text: itemModelData.label
        anchors.left: parent.left
        anchors.leftMargin: leftMarginOfLabel * units.scale
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 12 * units.fontScale
        Accessible.ignored: true
    }

    SwitchStyled {
        id: cellSwitch
        anchors.right: parent.right
        anchors.rightMargin: 10 * units.scale
        anchors.verticalCenter: parent.verticalCenter
        checked: itemModelData.value == "1"
        Accessible.ignored: true
        onClicked: {
            if (itemModelIndex != -1)
                settingsModel.cellSwitchValueChanged(itemModelIndex, checked);
        }
    }
}
