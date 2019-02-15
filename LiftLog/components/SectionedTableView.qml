import QtQuick 2.7
import LiftLog 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight
    color: "#ecf0f1"

    property var settingsModel
    property Component footerComponent
    property Component headerComponent
    property bool sectionAdditionalItemWillBeShown: false
    property var sectionAdditionalItemComponent: null
    property int sectionHeight: 35 * units.scale
    property bool centerVerticalSectionItems: false
    property bool showSections: true

    property alias cellWidth: listView.width
    property alias internalListView: listView

    Item {
        // When a tab gets active focus in a TabView, the first child of a TableView gets forced active focus
        // which might bring up the keyboard if the child is a TextInput, which we don't want to happen.
        // That's why we add a dummy focus scope before any children of the TableView.
        id: dummyFocusScope
        activeFocusOnTab: true
    }

    ListModel {
        id: testModel
        ListElement {
            label: "Weight unit"; section: "SETTINGS"; icon: "\ue614"; showIcon: true; showAccessory: true; accessoryIcon: "\ue60b"; value: "KG"; cellType: "text"
        }
        ListElement {
            label: "Bar & Plates"; section: "SETTINGS"; icon: "\ue600"; showIcon: true; showAccessory: true; accessoryIcon: "\ue60b"; value: ""; cellType: "text"
        }
        ListElement {
            label: "Timer"; section: "SETTINGS"; icon: "\ue952"; showIcon: true; value: "1"; cellType: "switch"
        }
        ListElement {
            section: "SETTINGS"; cellType: "slider"; value: "75"
        }
        ListElement {
            label: "Auto-add weight"; section: "SETTINGS"; icon: "\uf160"; showIcon: true; showAccessory: true; accessoryIcon: "\ue60b"; value: "On"; cellType: "text"
        }
        ListElement {
            label: "Sets & Reps"; section: "SETTINGS"; icon: "\ue2fc"; showIcon: true; showAccessory: true; accessoryIcon: "\ue60b"; value: ""; cellType: "text"
        }
        ListElement {
            label: "Backup"; section: "DATA"; icon: ""; showIcon: false; showAccessory: true; accessoryIcon: "\ue60b"; value: ""; cellType: "text"
        }
        ListElement {
            label: "Recover"; section: "DATA"; icon: ""; showIcon: false; showAccessory: true; accessoryIcon: "\ue60b"; value: ""; cellType: "text"
        }
        ListElement {
            label: "Export"; section: "DATA"; icon: ""; showIcon: false; showAccessory: true; accessoryIcon: "\ue60b"; value: ""; cellType: "text"
        }
        ListElement {
            label: "Restore"; section: "DATA"; icon: ""; showIcon: false; showAccessory: false; accessoryIcon: ""; value: ""; cellType: "text"
        }
        ListElement {
            label: "Reset"; section: "DATA"; icon: ""; showIcon: false; showAccessory: false; accessoryIcon: ""; value: ""; cellType: "text"
        }
    }

    FontLoader {
        id: icomoon
        source: "qrc:/assets/fonts/icomoon.ttf"
    }

    Component {
        id: cellTypeTextAndIcon
        CellTypeTextAndIcon {
            width: listView.width
            height: listView.cellHeight
        }
    }

    Component {
        id: cellTypeSwitch
        CellTypeSwitch {
            width: listView.width
            height: listView.cellHeight
        }
    }

    Component {
        id: cellTypeSlider
        CellTypeSlider {
            width: listView.width
            height: listView.cellHeight
        }
    }

    Component {
        id: cellTypeTextEdit
        CellTypeTextEdit {
            width: listView.width
            height: listView.cellHeight
        }
    }

    Component {
        id: cellTypeDoubleTextEdit
        CellTypeDoubleTextEdit {
            width: listView.width
            height: listView.cellHeight
        }
    }

    Component {
        id: cellTypeIncrement
        CellTypeIncrement {
            width: listView.width
            height: listView.cellHeight
        }
    }

    Component {
        id: cellTypeFake
        CellTypeBase {
            visible: false
            enabled: false
        }
    }

    Component {
        id: cellTypeDummy
        CellTypeBase {
        }
    }


    ListView {
        id: listView
        height: parent.height
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 6 * units.scale
        anchors.rightMargin: 6 * units.scale
        spacing: 0
        model: settingsModel

        focus: true
        highlightMoveDuration: 0
        flickDeceleration: 1000
        maximumFlickVelocity: 1000

        section.property: "section"
        section.criteria: ViewSection.FullString
        section.delegate: showSections ? sectionHeading : null

        Component {
            id: sectionHeading
            SectionedTableViewSection {
                id: sectionContainer
                width: listView.width
                sectionAdditionalItemWillBeShown: root.sectionAdditionalItemWillBeShown
                sectionAdditionalItemComponent: root.sectionAdditionalItemComponent
                sectionHeight: root.sectionHeight
                centerVerticalSectionItems: root.centerVerticalSectionItems
            }
        }

        property int cellHeight: 55 * units.scale
        property int deletingIndex: -1
        property var delegateWithActiveDeleteButton

        header: headerComponent ? headerComponent : null
        footer: footerComponent ? footerComponent : null

        delegate: Rectangle {
            id: cellWrapper
            width: listView.width
            height: listView.cellHeight
            color: model.cellType !== "fake" ? "white" : "transparent"
            property variant rowModel: model

            states: [
                State {
                    name: "pressed"
                    PropertyChanges {
                        target: cellWrapper; color: "#dde0e1"
                    }
                    PropertyChanges {
                        target: cellSeparator; color: "#dde0e1"
                    }
                    PropertyChanges {
                        target: cellReference; cellBackgroundColor: "#dde0e1"
                    }
                }
            ]

            onStateChanged: {
                if (state === "pressed") {
                    pressedMouseArea.enabled = false;
                } else {
                    mouseEnablingAnimation.start()
                }
            }

            MouseArea {
                id: pressedMouseArea
                anchors.fill: parent
                onPressed: {
                    if (model.cellType !== "switch" && model.cellType !== "slider" && model.cellType !== "doubleTextEdit" && model.cellType !== "textEdit" && model.cellType !== "increment"
                            && model.cellType !== "fake")
                        cellWrapper.state = "pressed"
                }
                onReleased: {
                    cellWrapper.state = ""
                }
                onClicked: {
                    if (model.cellType !== "switch" && model.cellType !== "slider" && model.cellType !== "doubleTextEdit" && model.cellType !== "textEdit" && model.cellType !== "increment"
                            && model.cellType !== "fake") {
                        settingsModel.cellClicked(index);
                    }
                }
                SequentialAnimation {
                    id: mouseEnablingAnimation
                    PauseAnimation { duration: 500 }
                    PropertyAction { target: pressedMouseArea; property: "enabled"; value: true }
                }
            }

            Connections {
                target: listView
                onMovingChanged: {
                    cellWrapper.state = ""
                }
            }

            // Holds reference to cell created by createObject.
            property var cellReference: null
            Component.onDestruction: {
                if (cellReference) {
                    cellReference.destroy();
                }
            }

            Component.onCompleted: {
                function componentChooser(type) {
                    if (type === "switch") return cellTypeSwitch;
                    if (type === "slider") return cellTypeSlider;
                    if (type === "text") return cellTypeTextAndIcon;
                    if (type === "doubleTextEdit") return cellTypeDoubleTextEdit;
                    if (type === "textEdit") return cellTypeTextEdit;
                    if (type === "increment") return cellTypeIncrement;
                    if (type === "fake") return cellTypeFake;

                    // Fix undefined errors.
                    console.warn("Trying to intantiate unknown cell type.");
                    return cellTypeDummy;
                }
                var component = componentChooser(model.cellType);
                var properties = {
                    itemModelData: Qt.binding(function() {
                        return model.properties;
                    }),
                    itemModelIndex: Qt.binding(function() {
                        return index;
                    }),
                    itemIsAllowedToShowDeleteButton: Qt.binding(function() {
                        return listView.deletingIndex === -1 || listView.deletingIndex === index;
                    }),
                };
                cellReference = component.createObject(cellWrapper, properties);
                cellConnections.enabled = true;
            }

            Connections {
                id: cellConnections
                target: cellReference
                enabled: false
                onItemIsShowingDeleteButton: {
                    listView.deletingIndex = deletingIndex
                    listView.delegateWithActiveDeleteButton = cellReference
                }
                onItemIsHidingDeleteButton: {
                    listView.deletingIndex = -1
                }
                onPleaseHideAllDeleteButtons: {
                    listView.delegateWithActiveDeleteButton.hideDeleteButton()
                }
                onPleaseUnfocus: {
                    dummyFocusScope.forceActiveFocus()
                }
            }

            Separator {
                id: cellSeparator
                anchors.bottom: parent.bottom
                separatorWidth: parent.width - 15 * units.scale
                separatorHeight: 1 * units.scale
                color: "#ecf0f1"
                separator.anchors.leftMargin: 15 * units.scale
            }
        }
    }

    function forceActiveFocusForFirstCell() {
        listView.currentIndex = 0;
        var next = listView.currentItem.nextItemInFocusChain(true);
        next.forceActiveFocus();
    }
}

