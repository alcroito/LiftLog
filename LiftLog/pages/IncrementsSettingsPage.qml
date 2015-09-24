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
            // Make sure not to show the rows initially when the page is loaded, in case if the auto-add weight option is disabled.
            if (!appState.currentUser.autoAddWeight) {
                incrementsModel.clearAllRows()
            }
        }
    }

    SectionedTableView {
        id: tableView
        anchors.top: parent.top
        anchors.topMargin: 10 * units.scale
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10 * units.scale
        settingsModel: incrementsModel

        // The header component will show a fake first section, and we disable regular sections
        // for the actual table.
        showSections: false
        headerComponent: SectionedTableViewSection {
            id: headerComponent
            manualSection: qsTr("INCREMENTS")
            width: tableView.internalListView.width
            sectionAdditionalItemWillBeShown: true
            sectionAdditionalItemComponent: additionalHeaderComponent
            sectionHeight: 50 * units.scale
            centerVerticalSectionItems: true
        }

        footerComponent: DialogTextBox {
            id: explanationText
            showDialog: true
            anchors.verticalCenter: undefined
            anchors.leftMargin: 0
            anchors.rightMargin: 0
            emptySpaceAbove: appState.currentUser.autoAddWeight ? 20 * units.scale : 0
            dialogText: {
                if (!appState.currentUser.autoAddWeight)
                    return qsTr("When increments are on, the app will automatically add weight every workout, if you successfully complete all reps on all sets.")
                else
                    return qsTr("How much weight should the app add each workout, if you complete the sets?")
            }
        }

        Component {
            id: additionalHeaderComponent
            SwitchStyled {
                id: autoIncrementsSwitch
                checked: appState.currentUser.autoAddWeight
                Accessible.ignored: true
                onClicked: {
                    // Save the auto-add weight setting.
                    appState.currentUser.autoAddWeight = checked
                    appState.saveCurrentUser()

                    // Depending on checked state, show or hide the actual weight increments.
                    if (checked) {
                        incrementsModel.init("increments")
                    } else {
                        incrementsModel.clearAllRows()
                    }

                    // Re-assign binding to the property, because it gets removed after click, by the internals of the component.
                    checked = Qt.binding(function() { return appState.currentUser.autoAddWeight; });
                }
            }
        }
    }

    Connections {
        target: modalPopup
        onAcceptClicked: {
            if (modalPopup.operation === modalPopup.popupResetUserIncrementsOperation) {
                // Re-enabled switch, if it was disabled.
                appState.currentUser.autoAddWeight = true
                appState.saveCurrentUser()

                // Reset to defaults.
                incrementsModel.resetToDefaults()
            }
        }
    }
}
