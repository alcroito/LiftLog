import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.4
import LiftLog 1.0
import "../components"

BasicPage {
    id: root
    showNavigationBarBackButton: true
    showNavigationRightResetButton: true
    navigationBar.onBackClicked: goBack();
    navigationBar.onResetClicked: {
        showModalPopup()
        modalPopup.prepare(modalPopup.popupResetUserIncrementsOperation)
    }
    onGoBack: {
        pageStack.goBack();
    }
    rootBackground.color: "#ecf0f1"
    width: appState.windowWidth
    height: appState.windowHeight

    onIsGoingToBeShown: {
        incrementsModel.refresh()
    }

    SettingsProxyModel {
        id: incrementsModel
        Component.onCompleted: {
            incrementsModel.init("increments");
        }
    }

    SectionedTableView {
        id: tableView
        anchors.top: parent.top
        anchors.topMargin: 10 * units.scale
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10 * units.scale
        settingsModel: incrementsModel
    }

    Connections {
        target: modalPopup
        onAcceptClicked: {
            if (modalPopup.operation === modalPopup.popupResetUserIncrementsOperation) {
                incrementsModel.resetToDefaults()
            }
        }
    }
}
