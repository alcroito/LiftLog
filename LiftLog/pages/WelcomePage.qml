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
            anchors.horizontalCenter: parent.horizontalCenter
            textFormat: Text.RichText
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 10 * units.fontScale
        }

        RowLayout {
            id: rowLayout3
            spacing: 20 * units.scale

            Label {
                id: label1
                text: qsTr("Name")
                font.pixelSize: 10 * units.fontScale
                Layout.minimumWidth: 100
            }

            TextField {
                id: textInput1
                Layout.fillWidth: true
                text: ""
            }
        }

        RowLayout {
            id: rowLayout1
            spacing: 20 * units.scale

            Label {
                id: label2
                text: qsTr("Unit System")
                font.pixelSize: 10 * units.fontScale
                Layout.minimumWidth: 100
            }


            ComboBox {
                id: comboBox1
                Layout.fillWidth: true

                property int metric: User.Metric

                model: ListModel {
                    id: cbItems

                    // Fix this to properly use Enums. It works in app code, but dies in
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
            id: rowLayout2
            spacing: 20 * units.scale

            Label {
                id: label3
                text: qsTr("Auto-add weight")
                font.pixelSize: 10 * units.fontScale
                Layout.minimumWidth: 100
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
            appState.currentUser.name = textInput1.text
            appState.currentUser.weightSystem = cbItems.get(comboBox1.currentIndex).value
            appState.currentUser.autoAddWeight = autoAddWeight.checked

            appState.saveCurrentUser()
            pageStack.showDashboardPage()
        }
    }
}

