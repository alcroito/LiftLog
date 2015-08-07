import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.3
import LiftLog 1.0
import "../components"

BasicPage {
    id: root

    property color borderColor: "#c6c6c6"
    property color redBorderColor: "#e74c3c"
    property alias weightText: weightInput.text

    // Incoming weightNumber is always in metric.
    property real weightNumber

    // 0 - Metric, 1 - Imperial
    property int weightSystem

    signal acceptedValues(real newUserWeight, int newWeightSystem)

    showNavigationBarDoneButton: true
    navigationBar.text: qsTr("Enter Body-weight")
    navigationBar.pixelSize: 10 * units.fontScale
    navigationBar.onDoneClicked: {
        forceActiveFocus()
        var newUserWeight = weightNumber
        var newWeightSystem = weightSystem
        acceptedValues(newUserWeight, newWeightSystem)
        goBack()
    }

    Rectangle {
        id: weightBackground
        radius: 4 * units.scale
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 40
        border.color: borderColor

        width: 120 * units.scale
        height: childrenRect.height

        TextField {
            id: weightInput
            height: 30 * units.scale
            width: 120 * units.scale
            text: appState.getWeightTransformed(weightNumber, User.Metric, weightSystem)
            horizontalAlignment: TextInput.AlignHCenter
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            maximumLength: 7
            validator: DoubleValidator {
                bottom: 1.0
                decimals: 2
                top: 999.0
                notation: DoubleValidator.StandardNotation
            }
            anchors.horizontalCenter: parent.horizontalCenter
            activeFocusOnPress: true
            focus: true
            onEditingFinished: {
                weightNumber = appState.getWeightTransformed(weightText, weightSystem, User.Metric)
            }

            style: TextFieldStyle {
                background: Item {}
            }
        }
    }

    Switch {
        id: weightSwitch
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: weightBackground.bottom
        anchors.topMargin: 10 * units.scale

        checked: weightSystem === 0 ? true : false

        onClicked: {
            // Force input before conversion, so onEditingFinished is executed.
            forceActiveFocus()

            var system = checked === true ? 0 : 1
            weightSystem = system
            weightText = appState.getWeightTransformed(weightNumber, User.Metric, system)

        }

        style: SwitchStyle {
            groove: Rectangle {
                id: baserect
                implicitWidth: weightBackground.width
                implicitHeight: weightBackground.height
                radius: 4 * units.scale
                border.color: redBorderColor
                border.width: 2 * units.scale

                Rectangle {
                    width: 58 * units.scale
                    height: parent.height
                    id: metricBackground
                    anchors.left: parent.left
                    anchors.leftMargin: 2 * units.scale

                    color: control.checked ? redBorderColor : "transparent"

                    Label {
                        anchors.centerIn: metricBackground
                        color: control.checked ? "white" : redBorderColor
                        text: "KG"
                    }
                }

                Rectangle {
                    width: 58 * units.scale
                    height: parent.height
                    id: imperialBackground
                    anchors.left: metricBackground.right

                    color: control.checked ? "transparent" : redBorderColor

                    Label {
                        anchors.centerIn: imperialBackground
                        color: control.checked ? redBorderColor : "white"
                        text: "LB"
                    }
                }
            }

            handle: Item {
                implicitWidth: 60 * units.scale
                implicitHeight: weightBackground.height
            }
        }
    }
}
