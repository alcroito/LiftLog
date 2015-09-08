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

        Item {
            id: cellTypeWrapper
            height: listView.cellHeight
            width: listView.width

            property var itemModelData
//            onItemModelDataChanged: {
//                console.log("data", itemModelData, itemModelIndex);
//            }
//            onItemModelIndexChanged: {
//                console.log("index", itemModelData, itemModelIndex);
//            }

            property int itemModelIndex: -1

            Loader {
                id: cellIconLoader

                anchors.left: parent.left
                anchors.leftMargin: 15 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                sourceComponent: Label {
                    id: cellIcon
                    font.family: icomoon.name
                    font.pixelSize: 12 * units.fontScale
                    text: itemModelData.icon
                    Accessible.ignored: true
                }
                active: itemModelData.showIcon
            }

            Label {
                property int leftMarginOfLabel: itemModelData.showIcon ? 40 : 10;
                id: cellLabel
                text: itemModelData.label
                anchors.left: parent.left
                anchors.leftMargin: leftMarginOfLabel * units.scale
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12 * units.fontScale
                Accessible.ignored: true
            }

            Label {
                id: cellValueLabel
                text: itemModelData.value
                color: "#c7c7cc"
                anchors.right:  parent.right
                anchors.rightMargin: 30 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12 * units.fontScale
                Accessible.ignored: true
            }

            Loader {
                id: cellAccessoryLoader

                anchors.right: parent.right
                anchors.rightMargin: 10 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                sourceComponent: Label {
                    id: cellAccessory
                    font.family: icomoon.name
                    font.pixelSize: itemModelData.selectable ? 16 * units.fontScale : 12 * units.fontScale
                    text: itemModelData.accessoryIcon
                    color: itemModelData.selectable ? "#e74c3c" : "#c7c7cc"
                    Accessible.ignored: true
                }
                active: itemModelData.selectable ? itemModelData.selected : itemModelData.showAccessory
            }
        }
    }

    Component {
        id: cellTypeSwitch

        Item {
            id: cellTypeWrapper
            height: listView.cellHeight
            width: listView.width
            property var itemModelData
            property int itemModelIndex: -1

            Loader {
                id: cellIconLoader

                anchors.left: parent.left
                anchors.leftMargin: 15 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                sourceComponent: Label {
                    id: cellIcon
                    font.family: icomoon.name
                    font.pixelSize: 12 * units.fontScale
                    text: itemModelData.icon
                    Accessible.ignored: true
                }
                active: itemModelData.showIcon
            }

            Label {
                property int leftMarginOfLabel: itemModelData.showIcon ? 40 : 10;
                id: cellLabel
                text: itemModelData.label
                anchors.left: parent.left
                anchors.leftMargin: leftMarginOfLabel * units.scale
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12 * units.fontScale
                Accessible.ignored: true
            }

            SwitchStyled {
                id: cellSwitch
                anchors.right: parent.right
                anchors.rightMargin: 10 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                checked: itemModelData.value == "1"
                Accessible.ignored: true
                onClicked: settingsModel.cellSwitchValueChanged(itemModelIndex, checked);
            }
        }
    }

    Component {
        id: cellTypeSlider

        Item {
            id: cellTypeWrapper
            height: listView.cellHeight
            width: listView.width
            property var itemModelData
            property int itemModelIndex: -1

            Loader {
                id: cellIconLoaderBefore

                anchors.left: parent.left
                anchors.leftMargin: 18 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                sourceComponent: Label {
                    id: cellIconBefore
                    font.family: icomoon.name
                    font.pixelSize: 12 * units.fontScale
                    text: "\uea29"
                    Accessible.ignored: true
                }
            }

            SliderStyled {
                id: cellSlider
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 35 * units.scale
                anchors.rightMargin: 50 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                minimumValue: 0.0
                maximumValue: 100.0
                stepSize: 1.0
                value: itemModelData.value
                updateValueWhileDragging: false
                Accessible.ignored: true
                onValueChanged: {
                    if (itemModelIndex != -1)
                        settingsModel.cellSliderValueChanged(itemModelIndex, value);
                }
            }

            Loader {
                id: cellIconLoaderAfter

                anchors.right: parent.right
                anchors.rightMargin: 15 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                sourceComponent: Label {
                    id: cellIconAfter
                    font.family: icomoon.name
                    font.pixelSize: 12 * units.fontScale
                    text: "\uea26"
                    Accessible.ignored: true
                }
            }
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

            MouseArea {
                id: pressedMouseArea
                anchors.fill: parent
                onPressed: {
                    if (model.cellType !== "switch" && model.cellType !== "slider")
                        cellWrapper.state = "pressed"
                }
                onReleased: {
                    cellWrapper.state = ""
                }
                onClicked: {
                    if (model.cellType !== "switch" && model.cellType !== "slider") {
                        settingsModel.cellClicked(index);
                    }
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
}

