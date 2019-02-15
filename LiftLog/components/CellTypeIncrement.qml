import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import LiftLog 1.0

CellTypeBase {
    id: cellTypeWrapper

    Label {
        id: cellLabel
        text: itemModelData.label
        anchors.left: parent.left
        anchors.leftMargin: 15 * units.scale
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 12 * units.fontScale
        Accessible.ignored: true
    }

    FontLoader {
        id: icomoon
        source: "qrc:/assets/fonts/icomoon.ttf"
    }

    RowLayout {
        id: valueContainer
        spacing: 0 * units.scale
        anchors.right: parent.right
        anchors.rightMargin: 15 * units.scale
        anchors.verticalCenter: parent.verticalCenter

        MouseArea {
            id: minusArea
            onClicked: {
                if (itemModelIndex != -1)
                    settingsModel.cellIncrementChanged(itemModelIndex, false);
            }
            height: cellTypeWrapper.height
            width: 30 * units.scale

            Label {
                id: minusLabel
                text: "\ue616"
                font.pixelSize: 10 * units.fontScale
                font.family: icomoon.name
                anchors.centerIn: parent
                Accessible.ignored: true
            }
        }

        Label {
            id: cellValueLabel
            text: itemModelData.value + itemModelData.displayValueSuffix
            font.pixelSize: 12 * units.fontScale
            horizontalAlignment: Text.AlignHCenter
            Accessible.ignored: true
            Layout.minimumWidth: 75 * units.scale
            Layout.alignment: Qt.AlignHCenter
        }

        MouseArea {
            id: plusArea
            onClicked: {
                if (itemModelIndex != -1)
                    settingsModel.cellIncrementChanged(itemModelIndex, true);
            }
            height: cellTypeWrapper.height
            width: 30 * units.scale

            Label {
                id: plusLabel
                text: "\ue615"
                font.pixelSize: 10 * units.fontScale
                font.family: icomoon.name
                anchors.centerIn: parent
                Accessible.ignored: true
            }
        }
    }
}
