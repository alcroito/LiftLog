import QtQuick 2.0
import QtQuick.Controls 1.4
import LiftLog 1.0

CellTypeBase {
    id: cellTypeWrapper

    Loader {
        id: cellIconLoaderBefore

        anchors.left: parent.left
        anchors.leftMargin: 18 * units.scale
        anchors.verticalCenter: parent.verticalCenter
        sourceComponent: Label {
            id: cellIconBefore
            font.family: icomoon.name
            font.pixelSize: 12 * units.fontScale
            text: "\uea29"
            Accessible.ignored: true
        }
    }

    SliderStyled {
        id: cellSlider
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 35 * units.scale
        anchors.rightMargin: 50 * units.scale
        anchors.verticalCenter: parent.verticalCenter
        minimumValue: 0.0
        maximumValue: 100.0
        stepSize: 1.0
        value: itemModelData.value
        updateValueWhileDragging: false
        Accessible.ignored: true
        onValueChanged: {
            if (itemModelIndex != -1)
                settingsModel.cellSliderValueChanged(itemModelIndex, value);
        }
    }

    Loader {
        id: cellIconLoaderAfter

        anchors.right: parent.right
        anchors.rightMargin: 15 * units.scale
        anchors.verticalCenter: parent.verticalCenter
        sourceComponent: Label {
            id: cellIconAfter
            font.family: icomoon.name
            font.pixelSize: 12 * units.fontScale
            text: "\uea26"
            Accessible.ignored: true
        }
    }
}
