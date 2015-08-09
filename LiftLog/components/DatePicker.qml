import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import LiftLog 1.0
import LiftLog.extras 1.0

import QtQuick.Controls.Private 1.0

Rectangle {
    width: appState.windowWidth
    height: 164 * units.scale
    property date initialDate: new Date()

    function getSelectedDate() {
        return datePickerModel.getDate(dateTumblerColumn.currentIndex)
    }

    Tumbler {
        id: dateTumbler
        anchors.fill: parent

        Component.onCompleted: {
            // Move the tumbler column to the middle element.
            dateTumbler.setCurrentIndexAt(0, datePickerModel.getDateIndex(initialDate))

            // Mar the column as initialized, which is used in a signal handler.
            dateTumblerColumn.initialized = true

            // Connect a signal handler to the PathView currentIndexChanged signal,
            // so we can update the model, without causing a binding loop (which would
            // happen, if we made a Connection on the TumblerColumn onCurrentIndexChanged singal.
            dateTumbler.__viewAt(0).onCurrentIndexChanged.connect(indexChangedHandler)
        }

        DatePickerModel {
            id: datePickerModel
            Component.onCompleted: {
                initWithDate(initialDate)
            }
        }

        TumblerColumn {
            id: dateTumblerColumn
            width: dateTumbler.width
            model: datePickerModel
            role: "text"
            property bool initialized: false
        }

        function indexChangedHandler() {
            if (dateTumblerColumn.initialized) {
                datePickerModel.update(dateTumblerColumn.currentIndex)
            }
        }

        style: TumblerStyle {
            id: tumblerStyle

            property color selectedCellBorderColor: "#cdcdcd"

            visibleItemCount: 7
            padding.left: 0
            padding.right: 0
            padding.top: 20 * units.scale
            padding.bottom: 20 * units.scale
            foreground: Item {}
            frame: Item {}
            highlight: Item {}
            separator: Item {}
            background: Item {}
            columnForeground: Item {
                Item {
                    anchors.centerIn: parent
                    width: parent.width
                    height: tumblerStyle.__delegateHeight

                    Rectangle {
                        width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: __frameHeight
                        color: selectedCellBorderColor
                        opacity: control.enabled ? 1 : 0.2
                        anchors.top: parent.top
                        visible: !styleData.activeFocus
                    }

                    Rectangle {
                        width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: __frameHeight
                        color: selectedCellBorderColor
                        opacity: control.enabled ? 1 : 0.2
                        anchors.top: parent.bottom
                        visible: !styleData.activeFocus
                    }
                }
            }
            delegate: Item {
                id: delegateItem
                implicitHeight: (control.height - padding.top - padding.bottom) / tumblerStyle.visibleItemCount

                Text {
                    id: label
                    text: styleData.value
                    color: control.enabled ? (styleData.activeFocus ? FlatStyle.focusedTextColor : FlatStyle.defaultTextColor) : FlatStyle.disabledColor
                    opacity: control.enabled ? enabledOpacity : FlatStyle.disabledOpacity
                    font.pixelSize: Math.round(TextSingleton.font.pixelSize * 1.3)
                    font.family: FlatStyle.fontFamily
                    renderType: FlatStyle.__renderType
                    anchors.centerIn: parent

                    readonly property real enabledOpacity: 1.1 - Math.abs(styleData.displacement * 2) / tumblerStyle.visibleItemCount * (230 / 255)

                    // Scale the Y axis of a cell, to be smaller, the farther away the cell is from the center.
                    transform: Matrix4x4 {
                            property real skewX: -Math.abs(Math.tan((Math.PI / 180) * 180 * displacementNormalized))
                            property real displacementNormalized: styleData.displacement / tumblerStyle.visibleItemCount
                            property real translateY: 1 - Math.abs(displacementNormalized)
                            matrix: Qt.matrix4x4(1, 0, 0, 1,
                                                 0, translateY, 0, 0,
                                                 0, 0, 1, 0,
                                                 0, 0, 0, 1)
                    }
                }

                Loader {
                    id: block
                    y: styleData.displacement < 0 ? 0 : (1 - offset) * parent.height
                    width: parent.width
                    height: parent.height * offset
                    clip: true
                    active: Math.abs(styleData.displacement) <= 1

                    property real offset: Math.max(0, 1 - Math.abs(styleData.displacement))

                    sourceComponent: Rectangle {
                        // Use a Rectangle that is the same color as the highlight in order to avoid rendering text on top of text.
                        color: styleData.activeFocus ? FlatStyle.highlightColor : "white"
                        anchors.fill: parent

                        Text {
                            id: focusText
                            y: styleData.displacement < 0 ? 0 : parent.height - height
                            width: parent.width
                            height: delegateItem.height
                            color: control.enabled ? (styleData.activeFocus ? "white" : FlatStyle.defaultTextColor) : FlatStyle.disabledColor
                            opacity: control.enabled ? 1 : FlatStyle.disabledOpacity
                            text: styleData.value
                            font.pixelSize: Math.round(TextSingleton.font.pixelSize * 1.5)
                            font.family: FlatStyle.fontFamily
                            renderType: FlatStyle.__renderType
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
        }
    }
}

