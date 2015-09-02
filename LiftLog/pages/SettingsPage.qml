import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import LiftLog 1.0
import "../components"

BasicPage {
    id: root
    showNavigationBarBackButton: true
    navigationBar.onBackClicked: {
        goBack()
    }
    rootBackground.color: "#ecf0f1"
    width: appState.windowWidth
    height: appState.windowHeight

    onIsGoingToBeShown: {
        model.refresh()
    }

    property string settingsPageId: "initial"

    SettingsModel {
        id: model
        Component.onCompleted: {
            model.getSettingsData(settingsPageId);
        }
        onSwitchToSettingsPage: {
            pageStack.showSpecificSettingsPage(pageId)
        }
    }

    SectionedTableView {
        id: tableView
        anchors.top: parent.top
        anchors.topMargin: 10 * units.scale
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10 * units.scale
        settingsModel: model
    }
}
