import QtQuick 2.4
import LiftLog 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight
    color: "#ecf0f1"

    property var settingsModel
    property var footerComponent
    property alias cellWidth: listView.width

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
        id: cellTypeDummy
        Item {
            anchors.fill: parent
        }
    }


    ListView {
        id: listView
        height: parent.height
//        boundsBehavior: Flickable.StopAtBounds
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 6 * units.scale
        anchors.rightMargin: 6 * units.scale
        spacing: 0
//        model: testModel
        model: settingsModel

        focus: true
        highlightMoveDuration: 0
        flickDeceleration: 1000
        maximumFlickVelocity: 1000

        section.property: "section"
        section.criteria: ViewSection.FullString
        section.delegate: sectionHeading

        property int cellHeight: 55 * units.scale

        footer: footerComponent ? footerComponent : null

        Component {
            id: sectionHeading
            Item {
                width: listView.width
                height: 35 * units.scale

                Label {
                    anchors.left: parent.left
                    anchors.leftMargin: 15 * units.scale
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 4 * units.scale
                    text: section
                    font.pixelSize: 9 * units.fontScale
                    color: "#6d6d72"
                }
            }
        }

        delegate: Rectangle {
            id: cellWrapper
            width: listView.width
            height: listView.cellHeight
            color: "white"
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
                    if (model.cellType !== "switch" && model.cellType !== "slider" && model.cellType !== "doubleTextEdit" && model.cellType !== "textEdit")
                        cellWrapper.state = "pressed"
                }
                onReleased: {
                    cellWrapper.state = ""
                }
                onClicked: {
                    if (model.cellType !== "switch" && model.cellType !== "slider" && model.cellType !== "doubleTextEdit" && model.cellType !== "textEdit") {
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

            // This currently does not work because itemModelData gets removed, before ListView delegate is being destryoed,
            // which causes delegate bindings to evaluate to errors. Figure this out by building Qt from source, and try to
            // find the cause of this. @TODO
//            Component.onCompleted: {
//                function componentChooser(type) {
//                    if (type === "switch") return cellTypeSwitch;
//                    if (type === "slider") return cellTypeSlider;
//                    if (type === "text") return cellTypeTextAndIcon;
//                    // Fix undefined errors.
//                    return cellTypeDummy;
//                }
//                var component = componentChooser(model.type);
//                component.createObject(cellWrapper, {itemModelData: Qt.binding(function() {
//                    console.log("Model binding changed");
//                    return model;
//                }), itemModelIndex: index});
//            }

            Loader {
                id: cellTypeLoader
                Binding {
                    id: binder
                    property: "itemModelData"
                    value: model.properties
                }
                Binding {
                    id: binder2
                    property: "itemModelIndex"
                    value: index
                }
                onLoaded: {
                    binder.target = cellTypeLoader.item
                    binder2.target = cellTypeLoader.item
                }

                sourceComponent: componentChooser(model.cellType);
                function componentChooser(type) {
                    if (type === "switch") return cellTypeSwitch;
                    if (type === "slider") return cellTypeSlider;
                    if (type === "text") return cellTypeTextAndIcon;
                    if (type === "doubleTextEdit") return cellTypeDoubleTextEdit;
                    if (type === "textEdit") return cellTypeTextEdit;
                    // Fix undefined errors.
                    console.warn("Trying to intantiate unknown cell type.");
                    return cellTypeDummy;
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

