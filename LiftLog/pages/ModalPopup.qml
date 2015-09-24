import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.3
import LiftLog 1.0
import "../components"

Item {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight

    enabled: false
    opacity: 0

    property color backgroundColor: "#dcdcdc"
    property color textColor: "#e1352d"
    property color borderColor: "#c6c6c6"
    property alias subjectText: subjectLabel.text
    property alias bodyText: bodyLabel.text
    property alias acceptButtonText: acceptButton.text
    property alias rejectButtonText: rejectButton.text
    property var disableComponent

    function prepare(op) {
        switch (op) {
            case popupDeleteOperation:
                subjectText = qsTr("Delete Workout")
                bodyText = qsTr("Are you sure you want to delete this workout?")
                operation = op
                break
            case popupDeleteDBOperation:
                subjectText = qsTr("Delete DB")
                bodyText = qsTr("Are you sure you want to delete the DB?")
                operation = op
                break
            case popupResetUserPlatesOperation:
                subjectText = qsTr("Reset")
                bodyText = qsTr("Do you want to restore the default plate weights?")
                operation = op
                break
            case popupResetUserBarbellsOperation:
                subjectText = qsTr("Reset")
                bodyText = qsTr("Do you want to restore the default barbell weights?")
                operation = op
                break
            case popupResetUserIncrementsOperation:
                subjectText = qsTr("Reset")
                bodyText = qsTr("Do you want to restore the default weight increments?")
                operation = op
                break
        }
    }

    property string operation
    property string popupDeleteOperation: "delete"
    property string popupDeleteDBOperation: "deletedb"
    property string popupResetUserPlatesOperation: "resetUserPlates"
    property string popupResetUserBarbellsOperation: "resetUserBarbells"
    property string popupResetUserIncrementsOperation: "resetUserIncrements"

    signal acceptClicked
    signal rejectClicked
    signal anyClicked

    Rectangle {
        id: background
        color: "#000000"
        anchors.fill: parent
        opacity: 0.5
    }

    Rectangle {
        id: popupBackground
        color: backgroundColor
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width: root.width * 0.96
        height: layout.height
        radius: 10 * units.scale

        Column {
            id: layout

            Item {
                width: popupBackground.width
                height: 16 * units.scale
            }

            Label {
                id: subjectLabel
                text: qsTr("Delete Workout")
                anchors.right: parent.right
                anchors.rightMargin: 20
                wrapMode: Text.WordWrap
                anchors.left: parent.left
                anchors.leftMargin: 10
                color: textColor
                font.bold: true
            }

            Separator {
                separatorWidth: popupBackground.width
                separatorHeight: 2 * units.scale
                separator.anchors.topMargin: 10 * units.scale
                separator.anchors.bottomMargin: 10 * units.scale
                color: textColor
            }

            Label {
                id: bodyLabel
                text: qsTr("Are you sure you want to delete this workout?")
                wrapMode: Text.WordWrap
                anchors.right: parent.right
                anchors.rightMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 10 * units.scale
            }

            Item {
                width: popupBackground.width
                height: 16 * units.scale
            }

            Separator {
                separatorWidth: popupBackground.width
                separatorHeight: 1 * units.scale
                color: borderColor
            }

            Row {
                id: row
                Button {
                    id: acceptButton
                    height: 50 * units.scale
                    width: popupBackground.width / 2
                    text: qsTr("Yes")

                    style: ButtonStyle {
                        background: Item {}
                    }

                    onClicked: {
                        root.acceptClicked()
                        root.anyClicked()
                    }
                }

                Separator {
                    separatorWidth: 1 * units.scale
                    separatorHeight: 50 * units.scale
                    color: borderColor
                }

                Button {
                    id: rejectButton
                    height: 50 * units.scale
                    width: popupBackground.width / 2
                    text: qsTr("No")

                    style: ButtonStyle {
                        background: Item {}
                    }

                    onClicked: {
                        root.rejectClicked()
                        root.anyClicked()
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "modalPopupShown"
            PropertyChanges {
                target: modalPopup
                enabled: true
                opacity: 1
            }

            PropertyChanges {
                target: disableComponent
                enabled: false
            }
        }
    ]

    transitions: [
        Transition {
            from: ""
            to: "modalPopupShown"
            PropertyAnimation {
                property: "opacity"
                easing.type: Easing.InQuad
                duration: 100
            }
        },
        Transition {
            from: "modalPopupShown"
            to: ""

            SequentialAnimation {
                PropertyAnimation {
                    property: "opacity"
                    duration: 100
                }

                PropertyAction {
                    target: modalPopupLoader; property: "active"; value: false
                }
            }
        }
    ]
}
