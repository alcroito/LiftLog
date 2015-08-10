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
            padding.top: 0 * units.scale
            padding.bottom: 0 * units.scale
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
                implicitHeight: (control.height) / tumblerStyle.visibleItemCount
                property real displacementNormalized: {
                    // For some reason the normalized displacement value is not always between -0.5 and 0.5,
                    // so we make sure to limit explicitly to the given range of values.
                    var t = styleData.displacement / tumblerStyle.visibleItemCount;
                    t = Math.max(-0.5, Math.min(t, 0.5))
                    return t;
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        // Move up or down one item, if clicking on a non-center item.
                        if (delegateItem.displacementNormalized > 0)
                            dateTumbler.__viewAt(0).decrementCurrentIndex();
                        else if (delegateItem.displacementNormalized < 0) dateTumbler.__viewAt(0).incrementCurrentIndex();
                    }
                }

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
//                    transform: Matrix4x4 {
////                            property real skewX: -Math.abs(Math.tan((Math.PI / 180) * 180 * label.displacementNormalized))
//                            property real translateY: 1 - Math.abs(label.displacementNormalized)
//                            matrix: Qt.matrix4x4(1, 0, 0, 1,
//                                                 0, translateY, 0, 0,
//                                                 0, 0, 1, 0,
//                                                 0, 0, 0, 1)
//                    }
                    // Create trapezoidal transform, the farther away an item is from the center cell.
                    layer.enabled: true
                    layer.smooth: true
                    layer.effect: ShaderEffect {
                        property real disp: delegateItem.displacementNormalized
                        mesh: GridMesh { resolution: Qt.size(1, 1) }
                        vertexShader: "
                            uniform highp mat4 qt_Matrix;
                            attribute highp vec4 qt_Vertex;
                            attribute highp vec2 qt_MultiTexCoord0;
                            varying highp vec2 qt_TexCoord0;
                            uniform highp float disp;
                            uniform highp float width;
                            uniform highp float height;
                            void main() {
                                qt_TexCoord0 = qt_MultiTexCoord0;
                                highp vec4 pos = qt_Vertex;
                                highp float t;
                                // Compute on which side should the item should be deformed into a trapezoid, if above center item, then the top segment
                                // of the rectangle should be smaller, if below center item, then the bottom segment should be made smaller.
                                if (disp > 0.0) { t = 1.0 - pos.y / height; }
                                else { t = pos.y / height; }
                                highp float xDir;

                                // Decide in which direction should x be displaced, depending if its on the left side of the center axis,
                                // or the right side.
                                if (pos.x == 0.0) { xDir = pos.x + 8.0; }
                                else if (pos.x == width) { xDir = pos.x - 8.0; }
                                else { xDir = pos.x; }
                                pos.x = mix(qt_Vertex.x, xDir, abs(disp) * t);

                                // Scale y coordinate to make the item squished, the farther away it is.
                                highp float yDir;
                                if (pos.y <= height / 2.0) { yDir = pos.y + 8.0; }
                                else { yDir = pos.y - 8.0; }
                                pos.y = mix(qt_Vertex.y, yDir, abs(disp));

                                gl_Position = qt_Matrix * pos;
                            }"
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
                        id: focusTextWrapper
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

