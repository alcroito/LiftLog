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

    Stack.onStatusChanged: {
        if (Stack.status == Stack.Active)
            calendar.refresh()
    }

    CalendarNavigation {
        id: navigation
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0

        onLeftClicked: {
            calendarDate = calendar.goPrevMonth()
        }

        onRightClicked: {
            calendarDate = calendar.goNextMonth()
        }
    }

    Calendar {
        id: calendar

        onMovedToPrevMonth: {
            navigation.calendarDate = newDate
        }

        onMovedToNextMonth: {
            navigation.calendarDate = newDate
        }

        onDateClicked: {
            pageStack.showSpecificWorkoutPage(date)
        }

        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: navigation.bottom
        anchors.topMargin: 0
    }
}
