import QtQuick 2.0
import LiftLog.pages 1.0

BottomPopupView {
    id: datePickerBottomPopupView
    enabled: false
    opacity: 0

    property alias initialDate: datePicker.initialDate
    property var disableComponent

    function getSelectedDate() {
        return datePicker.getSelectedDate()
    }

    DatePicker {
        id: datePicker
    }

    states: [
        State {
            name: "datePickerShown"
            PropertyChanges {
                target: datePickerBottomPopupView
                enabled: true
                opacity: 1
            }

            PropertyChanges {
                target: disableComponent
                enabled: false
            }

            PropertyChanges {
                target: popupBackground
                anchors.bottomMargin: 0
            }
        }
    ]

    transitions: [
        Transition {
            from: ""
            to: "datePickerShown"
            PropertyAnimation {
                target: popupBackground
                property: "anchors.bottomMargin"
                duration: 200
            }
        },
        Transition {
            from: "datePickerShown"
            to: ""

            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation {
                        property: "anchors.bottomMargin"
                        duration: 100
                        target: popupBackground
                    }
                    PropertyAnimation {
                        property: "opacity"
                        duration: 100
                    }
                }

                PropertyAction {
                    target: datePickerDialogLoader; property: "active"; value: false
                }
            }
        }
    ]
}

