import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import LiftLog 1.0

TextField {
    property string value: ""
    property string suffix: ""
    property bool validateAsWeight: false
    property bool validateAsInteger: false
    property string stringID: ""
    property real validatorBottomDouble: 0.0
    property real validatorTopDouble: 999.0

    signal valueModifiedAndIsDifferentOrIs0
    signal pleaseUnfocus

    property DoubleValidator weightValidator: DoubleValidator {
        bottom: validatorBottomDouble
        decimals: 2
        top: validatorTopDouble
        notation: DoubleValidator.StandardNotation
    }

    property IntValidator intValidator: IntValidator {
        bottom: 0
        top: 50
    }

    id: root
    text: value + suffix
    anchors.verticalCenter: parent.verticalCenter
    font.pixelSize: 12 * units.fontScale
    Accessible.ignored: true
    onActiveFocusChanged: {
//        console.log("active focus changed to ", activeFocus, " focus changed to ", focus, " for id ", stringID)
        if (activeFocus) {
            text = value
        } else {
            text = value + suffix
        }

    }
    onEditingFinished: {
        var oldValue = value
        value = text
//        console.log("editing finished active focus is", activeFocus, " focus is ", focus)
        text = value + suffix
        if (oldValue != value || value == 0) {
            valueModifiedAndIsDifferentOrIs0()
        }
    }
    Keys.onReturnPressed: {
        var next = root.nextItemInFocusChain(true);
//        next.forceActiveFocus();
        pleaseUnfocus()
    }

    inputMethodHints: {
//        if (validateAsWeight) return Qt.ImhFormattedNumbersOnly;
//        if (validateAsInteger) return Qt.ImhDigitsOnly;
        return Qt.ImhNone;
    }
    maximumLength: {
        if (validateAsWeight) return 7;
        if (validateAsInteger) return 3;
        return 200;
    }
    validator: {
        if (validateAsWeight) return weightValidator;
        if (validateAsInteger) return intValidator;
        return null;
    }

    style: TextFieldStyle {
        background: Rectangle {
            color: "transparent"
//            border.width: 1
//            border.color: "red"
        }
    }
}

