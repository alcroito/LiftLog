import QtQuick 2.0
import QtQuick.Controls 1.2
import LiftLog 1.0

Item {
    id: root
    width: appState.windowWidth
    height: 180 * units.scale

    property real exerciseWeight: 97.5
    property string labelText: "1 x 45LB\n2 x 10LB\n1 x 2.5LB\n1 x 10LB Barbell"
    onExerciseWeightChanged: {
        platesModel.switchWeight(exerciseWeight)
        labelText = platesModel.getPlatesBreakdownText()
    }

    Item {
        id: rowContainer
        anchors.topMargin: 37 * units.scale
        anchors.fill: parent
        height: 82 * units.scale
        anchors.leftMargin: 15 * units.scale

        Item {
            id: diagramContainer
            width: barbell.width
            height: 82 * units.scale

            Rectangle {
                id: barbell
                color: "#afafaf"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                width: 110 * units.scale
                height: 10 * units.scale
            }

            Rectangle {
                id: sleeve
                width: 10 * units.scale
                height: 26 * units.scale
                color: "#4e4e4e"
                anchors.right: platesList.left
                anchors.rightMargin: 0
                anchors.verticalCenter: parent.verticalCenter
                border.width: 1 * units.scale
                border.color: "#802a20"
            }

            ListModel {
                id: testModel
                ListElement {weight: 20}
                ListElement {weight: 10}
                ListElement {weight: 10}
                ListElement {weight: 5}
                ListElement {weight: 2.5}
                ListElement {weight: 1.25}
            }

            ExerciseWeightDiagramModel {
                id: platesModel
                Component.onCompleted: {
                    platesModel.switchWeight(exerciseWeight)
                }
            }

            ListView {
                function getPlateColor(weight) {
                    switch (weight) {
                        case 20:
                            return "#c74134";
                        case 10:
                            return "#b13b2d";
                        case 5:
                            return "#a8372c";
                        case 2.5:
                            return "#8a2d24";
                        case 1.25:
                            return "#8a2e23";
                        case 1:
                            return "#8a2e23";
                    }
                    return "#a8372c";
                }

                function getPlateHeight(weight) {
                    switch (weight) {
                        case 20:
                            return 80 * units.scale;
                        case 10:
                            return 50 * units.scale;
                        case 5:
                            return 40 * units.scale;
                        case 2.5:
                            return 30 * units.scale;
                        case 1.25:
                            return 20 * units.scale;
                        case 1:
                            return 25 * units.scale;
                    }
                    return 40 * units.scale;
                }

                id: platesList
                anchors.leftMargin: 24 * units.scale
                boundsBehavior: Flickable.StopAtBounds
                interactive: false
                orientation: ListView.Horizontal
                flickableDirection: Flickable.HorizontalFlick
                z: 2
                anchors.fill: parent
                model: platesModel
                delegate:
                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 10 * units.scale
                        height: platesList.getPlateHeight(model.weight)
                        color: platesList.getPlateColor(model.weight)
                        border.width: 1 * units.scale
                        border.color: "#802a20"
                    }
            }
        }

        Row {
            id: textAreaContainer
            anchors.left: diagramContainer.right
            anchors.leftMargin: 10 * units.scale
            anchors.verticalCenter: diagramContainer.verticalCenter

            Label {
                id: textArea
                text: labelText
            }
        }
    }
}

