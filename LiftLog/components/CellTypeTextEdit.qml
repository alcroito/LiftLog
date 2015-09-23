import QtQuick 2.5
import QtQuick.Controls 1.4
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

    CellTextInputWithSuffix {
        id: cellFirstTextInput
        anchors.right: parent.right
        anchors.rightMargin: 15 * units.scale
        horizontalAlignment: TextInput.AlignRight

        value: itemModelData.value1
        suffix: itemModelData.displayValue1Suffix
        stringID: "the X"
        validateAsWeight: itemModelData.value1ValidateAsWeight ? itemModelData.value1ValidateAsWeight : false
        validateAsInteger: itemModelData.value1ValidateAsInt ? itemModelData.value1ValidateAsInt : false
        onValueModifiedAndIsDifferent: {
            var textInputDelta = 0;
            if (itemModelIndex != -1)
                settingsModel.cellTextInputValueChanged(itemModelIndex, textInputDelta, value);
        }
    }
}
