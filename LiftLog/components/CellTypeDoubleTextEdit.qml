import QtQuick 2.5
import QtQuick.Controls 1.4
import LiftLog 1.0

CellTypeBase {
    id: cellTypeWrapper

    CellTextInputWithSuffix {
        id: cellFirstTextInput
        anchors.left: parent.left
        anchors.leftMargin: 15 * units.scale
        value: itemModelData.value1
        suffix: itemModelData.displayValue1Suffix
        validateAsWeight: itemModelData.value1ValidateAsWeight ? itemModelData.value1ValidateAsWeight : false
        validateAsInteger: itemModelData.value1ValidateAsInt ? itemModelData.value1ValidateAsInt : false
        width: 100 * units.scale
    }

    CellTextInputWithSuffix {
        id: cellSecondTextInput
        anchors.left: cellFirstTextInput.left
        anchors.leftMargin: 120 * units.scale
        value: itemModelData.value2
        suffix: itemModelData.displayValue2Suffix
        validateAsWeight: itemModelData.value1ValidateAsWeight ? itemModelData.value1ValidateAsWeight : false
        validateAsInteger: itemModelData.value1ValidateAsInt ? itemModelData.value1ValidateAsInt : false
        width: parent.width / 2
    }
}
