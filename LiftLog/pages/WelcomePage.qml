import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import LiftLog 1.0
import "../components"

BasicPage {
    id: root

    ColumnLayout {
        id: layout
        spacing: 15 * units.scale

        anchors.right: parent.right
        anchors.rightMargin: 20 * units.scale
        anchors.left: parent.left
        anchors.leftMargin: 20 * units.scale

        anchors.top: parent.top
        anchors.topMargin: 20 * units.scale

        Text {
            id: welcomeText
            text: qsTr("Welcome!<br/> Please fill out your profile to get started.")
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 10 * units.fontScale
        }

        RowLayout {
            id: rowLayout1
            spacing: 20 * units.scale

            Label {
                id: label1
                text: qsTr("Name")
                font.pixelSize: 10 * units.fontScale
                Layout.minimumWidth: 110 * units.scale
            }

            TextField {
                id: nameInput
                Layout.fillWidth: true
                text: ""
                activeFocusOnPress: true
                focus: true
            }
        }

        RowLayout {
            id: workoutLayout
            spacing: 20 * units.scale

            Label {
                id: workoutLabel
                text: qsTr("Initial workout")
                font.pixelSize: 10 * units.fontScale
                Layout.minimumWidth: 110 * units.scale
            }

            ComboBox {
                id: workoutComboBox
                Layout.fillWidth: true
                textRole: "name"
                model: WorkoutTemplateListModel {
                    id: workoutTemplateList
                }
                style: ComboBoxStyle {
                    font {
                        pixelSize: 10 * units.fontScale
                    }
                }
            }
        }

        RowLayout {
            id: rowLayout3
            spacing: 20 * units.scale

            Label {
                id: label2
                text: qsTr("Unit System")
                font.pixelSize: 10 * units.fontScale
                Layout.minimumWidth: 110 * units.scale
            }


            ComboBox {
                id: weightSystemComboBox
                Layout.fillWidth: true

                property int metric: User.Metric

                model: ListModel {
                    id: weightSystemModel

                    // @TODO Fix this to properly use Enums. It works in app code, but dies in
                    // Qt Quick Designer.
                    ListElement { text: "Metric"; value: 0 }
                    ListElement { text: "Imperial"; value: 1 }
                }

                style: ComboBoxStyle {
                    font {
                        pixelSize: 10 * units.fontScale
                    }
                }
            }
        }

        RowLayout {
            id: rowLayout4
            spacing: 20 * units.scale

            Label {
                id: label3
                text: qsTr("Auto-add weight")
                font.pixelSize: 10 * units.fontScale
                Layout.minimumWidth: 110 * units.scale
            }

            SwitchStyled {
                id: autoAddWeight
                checked: true
            }
        }
    }

    Button {
        text: "Save and Continue"
        anchors.horizontalCenterOffset: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20 * units.scale
        anchors.horizontalCenter: parent.horizontalCenter

        onClicked: {
            appState.currentUser.name = nameInput.text
            appState.currentUser.weightSystem = weightSystemModel.get(weightSystemComboBox.currentIndex).value
            appState.currentUser.autoAddWeight = autoAddWeight.checked
            appState.currentUser.lastIdWorkoutTemplate = workoutTemplateList.getItemDataForIndexAndRole(workoutComboBox.currentIndex, WorkoutTemplateListModel.IdRole)

            appState.saveCurrentUser()
            pageStack.showDashboardPage()
        }
    }
}

