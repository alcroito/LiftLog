import QtQuick 2.0
import LiftLog.pages 1.0

BottomPopupView {
    id: datePickerBottomPopupView
    enabled: false
    opacity: 0

    property alias initialDate: datePicker.initialDate

    function getSelectedDate() {
        return datePicker.getSelectedDate()
    }

    DatePicker {
        id: datePicker
    }
}

