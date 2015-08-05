import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import LiftLog 1.0
import "../components"

BasicPage {
    id: root

    showNavigationBarBackButton: true
    showNavigationBarSpreadsheetButton: true
    navigationBar.onBackClicked: goBack()

    CalendarModel {
        id: calendarModel
    }

    CalendarNavigation {
        id: navigation
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0

        onLeftClicked: {
            calendarModel.goPrevMonth()
            calendarDate = calendarModel.getDate()
        }

        onRightClicked: {
            calendarModel.goNextMonth()
            calendarDate = calendarModel.getDate()
        }
    }

    Calendar {
        id: calendar

        calendarModel: calendarModel

        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: navigation.bottom
        anchors.topMargin: 0
    }
}
