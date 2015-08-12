import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.3
import LiftLog 1.0
import "../components"

BasicPage {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight
    showNavigationBarDoneButton: true
    showNavigationLeftGearButton: true

    rootBackground.color: backgroundColor

    property color separatorColor: "#f0f0f0"
    property color backgroundColor: "#f9f9f9"
    property color borderColor: "#c6c6c6"
    property color redBorderColor: "#e74c3c"
    property alias weightText: weightInput.text
    property string overallStatusText: qsTr("%1 %2").arg(setsList.currentItem.text).arg(appState.getWeightString(weightNumber, exerciseEntity.isAccessory()))

    // Incoming weightNumber is always in metric.
    property real weightNumber

    // The set and rep id used for the exercise.
    property real setAndRepId

    // 0 - Metric, 1 - Imperial
    property int weightSystem

    // Weight increment with which to add / subtract when clicking on icons.
    // Will be adjusted for Imperial, but no further conversions are necessarry, the value is Metric.
    property real weightIncrement

    // 0 - Barbell
    // 1 - Body weight
    property int exerciseCategory: 0

    // The ExerciseEntity object for which we are changing the weight, etc.
    property var exerciseEntity

    // The index of the exercise in the workout exercise list view.
    property int exerciseIndex

    signal acceptedValues(real newExerciseWeight, int newSetAndRepId, string newSetsAndRepsString, int exerciseIndex)

    navigationBar.text: generateNavigationBarText()
    navigationBar.pixelSize: 14 * units.fontScale
    navigationBar.bold: true

    navigationBar.onDoneClicked: {
        forceActiveFocus()
        var newUserWeight = weightNumber
        var newSetAndRepId = setsList.currentItem.setAndRepId
        var newSetsAndRepsString = setsList.currentItem.text
        acceptedValues(newUserWeight, newSetAndRepId, newSetsAndRepsString, exerciseIndex)
        goBack()
    }

    Item {
        id: upperContainer
        width: parent.width
        height: 166 * units.scale
        anchors.top: parent.top
        anchors.topMargin: 0

        Loader {
            id: exerciseWeightDiagramLoader
            sourceComponent: ExerciseWeightDiagram {
                id: exerciseWeightDiagram
                exerciseWeight: weightNumber
            }
            active: exerciseEntity.isAccessory() !== true
        }
    }

    Rectangle {
        id: overallStatusContainer
        width: parent.width
        height: 42 * units.scale
        anchors.top: upperContainer.bottom
        anchors.topMargin: 0

        Separator {
            separatorWidth: root.width
            separatorHeight: 2 * units.scale
            color: separatorColor
            anchors.top: parent.top
            anchors.topMargin: 0
        }

        Label {
            id: overallStatusLabel
            text: overallStatusText
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        Separator {
            separatorWidth: root.width
            separatorHeight: 2 * units.scale
            color: separatorColor
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
        }
    }

    Item {
        id: weightContainer
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: overallStatusContainer.bottom
        anchors.topMargin: 40 * units.scale
        width: parent.width
        height: weightBackground.height

        FontLoader {
            id: icomoon
            source: "qrc:/assets/fonts/icomoon.ttf"
        }

        MouseArea {
            id: mouseArea1
            anchors.right: weightBackground.left
            anchors.rightMargin: 0 * units.scale

            width: iconMinus.width * 2
            height: weightBackground.height

            onClicked: {
                forceActiveFocus()
                if (weightNumber - weightIncrement >= weightInputValidator.bottom) {
                    weightNumber = weightNumber - weightIncrement
                }
            }

            Text {
                id: iconMinus
                font.family: icomoon.name
                text: qsTr("\uE608")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12 * units.fontScale
            }
        }

        Rectangle {
            id: weightBackground
            radius: 4 * units.scale
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
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
                    id: weightInputValidator
                    bottom: generateWeightInputLowerLimit()
                    decimals: 2
                    top: 999.0
                    notation: DoubleValidator.StandardNotation
                }
                anchors.horizontalCenter: parent.horizontalCenter
//                activeFocusOnPress: true
//                focus: true
                onEditingFinished: {
                    weightNumber = appState.getWeightTransformed(weightText, weightSystem, User.Metric)
                }

                style: TextFieldStyle {
                    background: Item {}
                }
            }
        }

        MouseArea {
            id: mouseArea2
            anchors.left: weightBackground.right
            anchors.leftMargin: 0 * units.scale

            width: iconPlus.width * 2
            height: weightBackground.height

            onClicked: {
                forceActiveFocus()
                if (weightNumber + weightIncrement <= weightInputValidator.top)
                    weightNumber = weightNumber + weightIncrement
            }

            Text {
                id: iconPlus
                font.family: icomoon.name
                text: qsTr("\uE609")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                font.pixelSize: 12 * units.fontScale
            }
        }
    }

    Item {
        id: setsContainer
        width: parent.width
        height: 44 * units.scale
        anchors.top: weightContainer.bottom
        anchors.topMargin: 32 * units.scale

        SetAndRepModel {
            id: setAndRepModel
            Component.onCompleted: {
                var index = setAndRepModel.initWithSetAndRepIdAndReturnSetIndex(exerciseEntity.getIdSetAndRep())
                setsList.currentIndex = index
                setsList.positionViewAtIndex(index, ListView.Center)
            }
        }

        VisualDataModel {
            id: setAndRepModelAndDelegate
            model: setAndRepModel
            delegate: SetAndRep {
                property int setAndRepId: model.setAndRepId
                state: model.state
                text: model.text
                whiteColor: "#f9f9f9"

                onClicked: {
                    setAndRepModel.resetStates()
                    setAndRepModel.setData(setAndRepModelAndDelegate.modelIndex(index), "active", SetAndRepModel.StateRole)
                    ListView.view.currentIndex = index
                }
            }
        }

        ListView {
            id: setsList
            boundsBehavior: Flickable.StopAtBounds
            anchors.rightMargin: 30 * units.scale
            anchors.leftMargin: 30 * units.scale
            anchors.fill: parent
            orientation: ListView.Horizontal
            spacing: 15 * units.scale
            model: setAndRepModelAndDelegate
            clip: true
        }
    }

    // Navigation bar text depends on:
    // Primary or accessory exercise (barbell vs body weight)
    // Either half on each side, or entire weight.
    // Lifting just body weight or just bar bell.
    // Assisted body weight exercise.
    function generateNavigationBarText() {
        var neat = true;
        switch (exerciseCategory) {
            case 0:
                // This if-break statement is for stopping a qml error when quitting the application, and the entity becomes null for some reason.
                // Probably the model gets unloaded, and the entity object gets deallocated, or something similar.
                if (!exerciseEntity) break;
                var barbellWeight = exerciseEntity.getBarbellWeight();
                if (weightNumber <= barbellWeight + 1.25) {
                    return qsTr("Lift the empty bar");
                } else {
                    return qsTr("Add %1/side").arg(appState.getWeightString((weightNumber - barbellWeight) / 2, false, false, true, neat));
                }
            case 1:
                if (weightNumber <= 0.1 && weightNumber >= -0.1) {
                    return qsTr("Lift yourself");
                } else if (weightNumber > 0) {
                    return qsTr("Add %1").arg(appState.getWeightString(weightNumber, false, false, true, neat));
                } else {
                    return qsTr("Assisted exercise");
                }
        }
        return qsTr("Enter weight");
    }

    // Returns the lower weight limit depending on the exercise category.
    function generateWeightInputLowerLimit() {
        switch (exerciseCategory) {
            case 0:
                return 0.0;
            case 1:
                return -100.0
        }
        return 0.0;
    }
}
