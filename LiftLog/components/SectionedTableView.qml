import QtQuick 2.4
import LiftLog 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
    id: root
    width: appState.windowWidth
    height: appState.windowHeight
    color: "#ecf0f1"

    ListModel {
        id: testModel
        ListElement {
            label: "Weight unit"; section: "SETTINGS"; icon: "\ue614"; showIcon: true; showAccessory: true; accessoryIcon: "\ue60b"; value: "KG"; type: "text"
        }
        ListElement {
            label: "Bar & Plates"; section: "SETTINGS"; icon: "\ue600"; showIcon: true; showAccessory: true; accessoryIcon: "\ue60b"; value: ""; type: "text"
        }
        ListElement {
            label: "Timer"; section: "SETTINGS"; icon: "\ue952"; showIcon: true; value: "1"; type: "switch"
        }
        ListElement {
            section: "SETTINGS"; type: "slider"; value: "75"
        }
        ListElement {
            label: "Auto-add weight"; section: "SETTINGS"; icon: "\uf160"; showIcon: true; showAccessory: true; accessoryIcon: "\ue60b"; value: "On"; type: "text"
        }
        ListElement {
            label: "Sets & Reps"; section: "SETTINGS"; icon: "\ue2fc"; showIcon: true; showAccessory: true; accessoryIcon: "\ue60b"; value: ""; type: "text"
        }
        ListElement {
            label: "Backup"; section: "DATA"; icon: ""; showIcon: false; showAccessory: true; accessoryIcon: "\ue60b"; value: ""; type: "text"
        }
        ListElement {
            label: "Recover"; section: "DATA"; icon: ""; showIcon: false; showAccessory: true; accessoryIcon: "\ue60b"; value: ""; type: "text"
        }
        ListElement {
            label: "Export"; section: "DATA"; icon: ""; showIcon: false; showAccessory: true; accessoryIcon: "\ue60b"; value: ""; type: "text"
        }
        ListElement {
            label: "Restore"; section: "DATA"; icon: ""; showIcon: false; showAccessory: false; accessoryIcon: ""; value: ""; type: "text"
        }
        ListElement {
            label: "Reset"; section: "DATA"; icon: ""; showIcon: false; showAccessory: false; accessoryIcon: ""; value: ""; type: "text"
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

            property var itemModelData: null
            property int itemModelIndex: -1

            Loader {
                id: cellIconLoader

                anchors.left: parent.left
                anchors.leftMargin: 15 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                sourceComponent: Label {
                    id: cellIcon
                    font.family: icomoon.name
                    font.pixelSize: 18 * units.scale
                    text: itemModelData.icon
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
                font.pixelSize: 16 * units.scale
            }

            Label {
                id: cellValueLabel
                text: itemModelData.value
                color: "#c7c7cc"
                anchors.right:  parent.right
                anchors.rightMargin: 30 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 16 * units.scale
            }

            Loader {
                id: cellAccessoryLoader

                anchors.right: parent.right
                anchors.rightMargin: 10 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                sourceComponent: Label {
                    id: cellAccessory
                    font.family: icomoon.name
                    font.pixelSize: 16 * units.scale
                    text: itemModelData.accessoryIcon
                    color: "#c7c7cc"
                }
                active: itemModelData.showAccessory
            }
        }
    }

    Component {
        id: cellTypeSwitch

        Item {
            id: cellTypeWrapper
            height: listView.cellHeight
            width: listView.width
            property var itemModelData: null
            property int itemModelIndex: -1

            Loader {
                id: cellIconLoader

                anchors.left: parent.left
                anchors.leftMargin: 15 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                sourceComponent: Label {
                    id: cellIcon
                    font.family: icomoon.name
                    font.pixelSize: 18 * units.scale
                    text: itemModelData.icon
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
                font.pixelSize: 16 * units.scale
            }

            SwitchStyled {
                id: cellSwitch
                anchors.right: parent.right
                anchors.rightMargin: 10 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                checked: itemModelData.value === "1"
            }
        }
    }

    Component {
        id: cellTypeSlider

        Item {
            id: cellTypeWrapper
            height: listView.cellHeight
            width: listView.width
            property var itemModelData: null
            property int itemModelIndex: -1

            Loader {
                id: cellIconLoaderBefore

                anchors.left: parent.left
                anchors.leftMargin: 18 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                sourceComponent: Label {
                    id: cellIconBefore
                    font.family: icomoon.name
                    font.pixelSize: 18 * units.scale
                    text: "\uea29"
                }
            }

            SliderStyled {
                id: cellSlider
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 35 * units.scale
                anchors.rightMargin: 50 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                minimumValue: 0
                maximumValue: 100
                value: itemModelData.value
            }

            Loader {
                id: cellIconLoaderAfter

                anchors.right: parent.right
                anchors.rightMargin: 15 * units.scale
                anchors.verticalCenter: parent.verticalCenter
                sourceComponent: Label {
                    id: cellIconAfter
                    font.family: icomoon.name
                    font.pixelSize: 18 * units.scale
                    text: "\uea26"
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
        model: testModel

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
                    if (model.type !== "switch" && model.type !== "slider")
                    cellWrapper.state = "pressed"
                }
                onReleased: {
                    cellWrapper.state = ""
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
                    value: model
                }
                onLoaded: {
                    binder.target = cellTypeLoader.item
                }

                sourceComponent: componentChooser(model.type);
                function componentChooser(type) {
                    if (type === "switch") return cellTypeSwitch;
                    if (type === "slider") return cellTypeSlider;
                    if (type === "text") return cellTypeTextAndIcon;
                    // Fix undefined errors.
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

