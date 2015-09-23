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
        modalPopup.prepare("resetUserPlates")
    }

    onGoBack: {
        // Force active focus, to make sure that if any text field was focused and edited, it saves its value.
        navigationBar.forceActiveFocus();
        pageStack.goBack();
    }
    rootBackground.color: "#ecf0f1"
    width: appState.windowWidth
    height: appState.windowHeight

    onIsGoingToBeShown: {
        platesModel.refresh()
        barbellModel.refresh()
    }

    SettingsProxyModel {
        id: platesModel
        Component.onCompleted: {
            platesModel.init("plates");
        }
    }

    SettingsModel {
        id: barbellModel
        Component.onCompleted: {
            barbellModel.init("barbell");
        }
    }

    FontLoader {
        id: icomoon
        source: "qrc:/assets/fonts/icomoon.ttf"
    }

    TabView {
        id: tabView
        tabPosition: Qt.BottomEdge
        frameVisible: false
        anchors.fill: parent
        style: TabViewStyleIOS {

        }

        Tab {
            title: "Plates"
            property string tabIcon: "\ue2fe"
            SectionedTableView {
                id: tableView
                anchors.top: parent.top
                anchors.topMargin: 10 * units.scale
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10 * units.scale
                settingsModel: platesModel
                footerComponent: Component {
                    FinishWorkoutButton {
                        noMargins: true
                        width: tableView.cellWidth
                        height: 42 * units.scale
                        text: qsTr("Add Plate")
                        onClicked: {
                            platesModel.prependNewRow()
                            tableView.forceActiveFocusForFirstCell()
                        }
                    }
                }
            }
        }
        Tab {
            title: "Barbell"
            property string tabIcon: "\ue600"
            SectionedTableView {
                id: tableView2
                anchors.top: parent.top
                anchors.topMargin: 10 * units.scale
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10 * units.scale
                settingsModel: barbellModel
            }
        }
    }

    Connections {
        target: modalPopup
        onAcceptClicked: {
            if (modalPopup.operation === modalPopup.popupResetUserPlatesOperation) {
                platesModel.resetToDefaults()
            }
        }
    }
}
