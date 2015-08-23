import QtQuick 2.4
import QtQuick.Controls 1.2
import LiftLog 1.0

Item {
    id: root
    width: appState.windowWidth
    height: 318 * units.scale

    property int segments: 4
    property int xLower: 0
    property int xUpper: 90
    property real xSegmentRange: 10
    property int yLower: 0
    property int yUpper: 90
    property real ySegmentRange: 10
    property var bounds
    property int selectedPeriod: 0

    StatsGraphData {
        id: statsData
        period: selectedPeriod

        onPeriodChanged: {
            var lowerAndUpper = statsData.getLowerAndUpperBounds()
            segments = statsData.getBestSegmentCount();
            bounds = lowerAndUpper
            computeXAndYAxisRanges(bounds)
            graphGrid.requestPaint()
        }

        Component.onCompleted: {
            var lowerAndUpper = statsData.getLowerAndUpperBounds()
            segments = statsData.getBestSegmentCount();
            bounds = lowerAndUpper
            computeXAndYAxisRanges(bounds)
        }
    }

    ListModel {
        id: testModel

        ListElement {
            name: "Squat"
            color: "blue"
            points: [
                ListElement {xValue: 1434067200; yValue: 20},
                ListElement {xValue: 1434326400; yValue: 22.5},
                ListElement {xValue: 1434672000; yValue: 22.5},
                ListElement {xValue: 1434931200; yValue: 25},
                ListElement {xValue: 1436054400; yValue: 30},
                ListElement {xValue: 1436486400; yValue: 32.5}
            ]
        }

        ListElement {
            name: "Bench"
            color: "red"
            points: [
                ListElement {xValue: 1434067200; yValue: 40},
                ListElement {xValue: 1434326400; yValue: 45},
                ListElement {xValue: 1434672000; yValue: 50},
                ListElement {xValue: 1434931200; yValue: 50},
                ListElement {xValue: 1436054400; yValue: 50},
                ListElement {xValue: 1436486400; yValue: 45}
            ]
        }


        Component.onCompleted: {
//            var lowerAndUpper = findLowerAndUpperBounds(testModel)
//            bounds = lowerAndUpper
//            computeXAndYAxisRanges(bounds)
        }
    }

    Repeater {
        id: xLabels

        model: segments
        Label {
            property int value: xLower + index * xSegmentRange
            anchors.left: graphGrid.left
            anchors.top: graphGrid.bottom
            anchors.topMargin: 12 * units.scale
            anchors.leftMargin: graphGrid.xWidth * index - contentWidth / 2
            text: timestampToDateFormatter(value)
            color: "#6e6e6e"
        }
    }

    Repeater {
        id: yLabels

        model: segments
        Label {
            property int value: yUpper - index * ySegmentRange
            anchors.right: graphGrid.left
            anchors.top: graphGrid.top
            anchors.rightMargin: 2 * units.scale
            anchors.topMargin: graphGrid.yHeight * index - contentHeight / 2
            text: appState.getWeightStringBuilder(value).setLowerCase(true).get();
            color: "#6e6e6e"
            font.pixelSize: 7 * units.fontScale
        }
    }

    Canvas {
        id: graphGrid
        anchors.bottomMargin: 22 * units.scale
        anchors.topMargin: 5 * units.scale
        anchors.rightMargin: 20 * units.scale
        anchors.leftMargin: 40 * units.scale
        anchors.fill: parent

        antialiasing: true
        renderTarget: Canvas.Image
        renderStrategy: Canvas.Immediate

        // Drawable width and height, which might include some padding.
        property int drawableWidth: width - widthPadding * 2
        property int drawableHeight: height
        property int widthPadding: 10 * units.scale

        // Length of cell segments.
        property int xWidth: Math.round(width / (segments - 1))
        property int yHeight: Math.round(height / (segments - 1))

        property bool drawPoints: false
        property bool lineChosen: false
        property int chosenLineIndex: -1

        property color borderColor: "#dddddd"
        property real outerLineOpacity: dragHandle.lineOpacity

        Rectangle {
            id: dragHandle
            width: 20 * units.scale
            height: graphGrid.height
            gradient: Gradient {
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: 1.0; color: "#8c498e" }
            }

            opacity: 0
            property real lineOpacity: 1.0

            states: [
                State {
                    name: "shown"
                    PropertyChanges {
                        target: dragHandle; opacity: 1; lineOpacity: 0.5;
                    }
                }
            ]

            transitions: Transition {
                NumberAnimation {
                    properties: "opacity,lineOpacity"
                    duration: 200
                }
            }

            Rectangle {
                id: dragBubble
                radius: 5 * units.scale
                width: 90 * units.scale
                height: 50 * units.scale
                color: "#8c498e"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: -5 * units.scale
                opacity: 0

                states: [
                    State {
                        name: "shown"
                        PropertyChanges {
                            target: dragBubble; opacity: 0.8
                        }
                    }
                ]

                transitions: Transition {
                    NumberAnimation {
                        property: "opacity"
                        duration: 200
                    }
                }

                Label {
                    id: bubbleText
                    anchors.centerIn: dragBubble
                    color: "white"
                    text: "hi"
                }

                Triangle {
                    id: bubbleTriangle
                    anchors.top: dragBubble.bottom
                    anchors.horizontalCenter: dragBubble.horizontalCenter
                    width: 10 * units.scale
                    height: 5 * units.scale
                    triangleWidth: 10 * units.scale
                    triangleHeight: 5 * units.scale
                    strokeStyle: "#8c498e"
                    fillStyle: "#8c498e"
                    fill: true
                }
            }
        }

        MouseArea {
            id: dragMouseArea
            anchors.fill: parent
            drag.target: dragHandle
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX: graphGrid.width - dragHandle.width
            drag.smoothed: false
            property int exerciseIndex: 0

            function updateViews(positionData) {
                bubbleText.text = positionData.bubbleText
                dragHandle.gradient.stops[1].color = positionData.color
                dragBubble.color = positionData.color
                bubbleTriangle.strokeStyle = positionData.color
                bubbleTriangle.fillStyle = positionData.color
                exerciseIndex = positionData.exerciseIndex
                if (positionData.closeEnoughToPoint) {
                    dragBubble.state = "shown"
                } else {
                    dragBubble.state = ""
                }
            }

            onPressed: {
                // Show the handle, and place it at the middle of the cursor.
                dragHandle.state = "shown"
                dragHandle.x = mouse.x - dragHandle.width / 2

                var transformedPoint = graphGrid.mapFromPoint(Qt.point(mouse.x, mouse.y))
                var positionData = statsData.getNearestPointAndExerciseData(transformedPoint)

                // Set a chosn line.
                graphGrid.chosenLineIndex =  positionData.exerciseIndex
                graphGrid.lineChosen = true

                updateViews(positionData)

            }
            onPositionChanged: {
                var transformedPoint = graphGrid.mapFromPoint(Qt.point(mouse.x, mouse.y))
                var positionData = statsData.getNearestPointAndExerciseData(transformedPoint, exerciseIndex)
                updateViews(positionData)
            }
            onReleased: {
                // Remove chosen line.
                graphGrid.lineChosen = false
                dragHandle.state = ""
            }
        }

        // Maps x from [xLower, xUpper] interval to [newXLower, newXUpper].
        function mapValue(xLower, xUpper, newXLower, newXUpper, x) {
            var range = xLower;
            var x1 = xLower - range;
            var x2 = xUpper - range;

            var newRange = newXLower;
            var nx1 = newXLower - newRange;
            var nx2 = newXUpper - newRange;

            var ratio = nx2 / x2;
            var xProj = (x - xLower) * ratio + newXLower;
            return xProj;
        }

        // Maps point to grid, specifically to drawable rectangle coordinates (which allows for padding on the left and right side).
        // Also aplies half of the line width, to adjust sub-pixel drawing that is relevant for Canvas 2D operations.
        function mapPoint(point, lineWidth) {
            var x = mapValue(xLower, xUpper, widthPadding, drawableWidth + widthPadding, point.x);
            var y = mapValue(yLower, yUpper, 0, drawableHeight, point.y);
            // Round to ints, so there are no sub-pixel artifacts.
            x = Math.round(x)
            y = Math.round(y)
            x = Math.min(x, drawableWidth + widthPadding - lineWidth / 2)
            y = Math.min(y, drawableHeight - lineWidth / 2)
            return Qt.point(x, y);
        }

        // Maps from a grid point, back to an interval point defined by the data.
        function mapFromPoint(point) {
            var x = mapValue(widthPadding, drawableWidth + widthPadding, xLower, xUpper, point.x);
            // The first two arguments are inversed on purpose.
            var y = mapValue(drawableHeight, 0, yLower, yUpper, point.y);
            return Qt.point(x, y);
        }

        onOuterLineOpacityChanged: {
            requestPaint()
        }

        onPaint: {
            var ctx = graphGrid.getContext('2d')

            // Reset canvas.
            ctx.clearRect(0, 0, width, height)
            ctx.globalAlpha = 1.0

            ctx.lineWidth = 1 * units.scale
            ctx.strokeStyle = borderColor
//            console.log("Width X Height", drawableWidth, drawableHeight)

            // Grid lines.
            var deltaForSubPixelPrecision = ctx.lineWidth / 2;
            var d = deltaForSubPixelPrecision;
            for (var i = 0; i < segments; ++i) {
                ctx.beginPath()
                var xWidthI = xWidth * i;
                xWidthI = Math.min(width - d, xWidthI + d) ;
                ctx.moveTo(xWidthI, 0)
                ctx.lineTo(xWidthI, height)
                ctx.closePath()
                ctx.stroke()

                var yHeightI = yHeight * i;
                yHeightI = Math.min(height - d, yHeightI + d)
                ctx.beginPath()
                ctx.moveTo(0, yHeightI)
                ctx.lineTo(width, yHeightI)
                ctx.closePath()
                ctx.stroke()
            }

            // Plot lines.
            for (var l = 0; l < statsData.exerciseCount(); l++) {
                var linePointsCount = statsData.pointCount(l)

                ctx.lineWidth = 2 * units.scale
                ctx.strokeStyle = statsData.exerciseColor(l)
                ctx.miterLimit = 100
                ctx.lineJoin = "miter"

                // Draw with different transparency, whenever a line is selected.
                if (lineChosen) {
                    if (l === chosenLineIndex) {
                        ctx.globalAlpha = 1.0
                    }
                    else {
                        ctx.globalAlpha = outerLineOpacity
                    }
                } else {
                    ctx.globalAlpha = 1.0
                }
//                console.log(ctx.globalAlpha)

                // Plot values.
                if (linePointsCount >= 1) {
                    var firstPoint = statsData.getCoordinatePoint(l, 0)
                    var mappedPoint = mapPoint(firstPoint, ctx.lineWidth);
//                    console.log(mappedPoint.x, mappedPoint.y);

                    ctx.beginPath()

                    // Initial point will create a fake segment, to make the line look just a bit nicer.
                    ctx.moveTo(widthPadding / 2, height - mappedPoint.y)
                    ctx.lineTo(mappedPoint.x, height - mappedPoint.y)

                    // Display a rect for each point.
                    if (drawPoints) {
                        ctx.rect(mappedPoint.x, height - mappedPoint.y, 2, 2);
                        ctx.fill();
                    }

                    for (i = 1; i < linePointsCount; ++i) {
                        var point = statsData.getCoordinatePoint(l, i)
                        mappedPoint = mapPoint(point, ctx.lineWidth);
                        ctx.lineTo(mappedPoint.x, height - mappedPoint.y);
                        if (drawPoints) {
                            ctx.rect(mappedPoint.x, height - mappedPoint.y, 2, 2);
                            ctx.fill();
                        }

//                        console.log(mappedPoint.x, mappedPoint.y);
                    }
                    // Last point will create a fake segment, to make the line look just a bit nicer.
                    ctx.lineTo(width - widthPadding / 2, height - mappedPoint.y);
                    ctx.stroke()
                }
            }
        }
    }

    function log10(val) {
      return Math.log(val) / Math.LN10;
    }

    function findLowerAndUpperBounds(list) {
        var xLower = Number.MAX_VALUE, xUpper = Number.MIN_VALUE;
        var yLower = Number.MAX_VALUE, yUpper = Number.MIN_VALUE;
        for (var i = 0; i < list.count; i++) {
            var exercise = list.get(i);
            for (var j = 0; j < exercise.points.count; j++) {
                var obj = exercise.points.get(j);
                if (obj.xValue < xLower) {
                    xLower = obj.xValue;
                }
                if (obj.xValue > xUpper) {
                    xUpper = obj.xValue;
                }

                if (obj.yValue < yLower) {
                    yLower = obj.yValue;
                }
                if (obj.yValue > yUpper) {
                    yUpper = obj.yValue;
                }
            }
        }

        return {xLower: xLower, xUpper: xUpper, yLower: yLower, yUpper: yUpper};
    }

    function incrementTickNicely(unroundedTickSize) {
        var x = Math.ceil(log10(unroundedTickSize));
        var pow10x = Math.pow(10, x);

        // Is between 0.1 and 1.0.
        var unitTickSize = unroundedTickSize / pow10x;
        //unitTickSize += 0.5;
        unitTickSize *= 2;

        var roundedTickRange = unitTickSize * pow10x;

        return roundedTickRange;
    }

    function makeNeatTickSize(unroundedTickSize) {
        var x = Math.ceil(log10(unroundedTickSize));
        var pow10x = Math.pow(10, x);

        // Is between 0.1 and 1.0
        var unitTickSize = unroundedTickSize / pow10x;

        // Round to a nice value.
        var adjustedTickSize = 0;
        if (unitTickSize <= 0.1) adjustedTickSize = 0.1;
//            else if (unitTickSize <= 0.2) adjustedTickSize = 0.2;
        else if (unitTickSize <= 0.25) adjustedTickSize = 0.25;
//            else if (unitTickSize <= 0.3) adjustedTickSize = 0.3;
//            else if (unitTickSize <= 0.4) adjustedTickSize = 0.4;
        else if (unitTickSize <= 0.5) adjustedTickSize = 0.5;
//            else if (unitTickSize <= 0.6) adjustedTickSize = 0.6;
//            else if (unitTickSize <= 0.7) adjustedTickSize = 0.7;
        else if (unitTickSize <= 0.75) adjustedTickSize = 0.75;
//            else if (unitTickSize <= 0.8) adjustedTickSize = 0.8;
//            else if (unitTickSize <= 0.9) adjustedTickSize = 0.9;
        else if (unitTickSize <= 1.0) adjustedTickSize = 1.0;

        var roundedTickRange = adjustedTickSize * pow10x;

        // Make sure the tick range is at least 10, for friendlier results.
        if (roundedTickRange < 10) roundedTickRange = 10;
        return roundedTickRange;
    }

    function computeBoundsAndTickRange(lower, upper, ticks, withRounding) {
        // Compute range.
        var range = upper - lower;

        // Compute tick size.
        var unroundedTickSize = range / (ticks - 1);
        var roundedTickRange = unroundedTickSize

        // Round the tick if requested.
        if (withRounding) {
            roundedTickRange = makeNeatTickSize(unroundedTickSize);

            // Compute rounded lower and upper bounds;
            var oldUpper = upper;
            var oldLower = lower;
            lower = roundedTickRange * Math.floor(lower / roundedTickRange);
            upper = lower + roundedTickRange * (ticks - 1) ;

            if (lower === oldLower && (lower - roundedTickRange) >=0) {
                lower -= roundedTickRange;
                upper -= roundedTickRange;
            }

            var tries = 5
            // If the new upper value does not include the old upper value in the respective interval,
            // increase the tick range, until it does include the old upper value.
            while (upper <= oldUpper || lower >= oldLower) {
                if (tries == 0) break;
                // Re-adjust tick to be bigger.
                roundedTickRange = incrementTickNicely(roundedTickRange)
                lower = roundedTickRange * Math.floor(lower / roundedTickRange);
                upper = lower + roundedTickRange * (ticks - 1) ;
                tries--;
            }
        }

        return {lower: lower, upper: upper, tickRange: roundedTickRange};
    }

    function computeXAndYAxisRanges(lowerAndUpper) {
        // x axis bounds
        var newBounds = computeBoundsAndTickRange(lowerAndUpper.xLower, lowerAndUpper.xUpper, segments, false)
        xLower = newBounds.lower;
        xUpper = newBounds.upper;
        xSegmentRange = newBounds.tickRange;
        console.log(xLower, xUpper, xSegmentRange);

        // y axis bounds
        newBounds = computeBoundsAndTickRange(lowerAndUpper.yLower, lowerAndUpper.yUpper, segments, true)
        yLower = newBounds.lower;
        yUpper = newBounds.upper;
        ySegmentRange = newBounds.tickRange;
        console.log(yLower, yUpper, ySegmentRange);
    }

    function timestampToDateFormatter(timestampInSecs) {
        var date = new Date(timestampInSecs * 1000)
        var formattedDate = Qt.formatDateTime(date, "dd/MM")
        return formattedDate
    }
}

