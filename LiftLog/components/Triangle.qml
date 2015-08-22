import QtQuick 2.4
import LiftLog 1.0

Canvas {
    id: triangle
    antialiasing: true

    property int triangleWidth: 60 * units.scale
    property int triangleHeight: 60 * units.scale
    property color strokeStyle: "#ffffff"
    property color fillStyle: "#ffffff"
    property int lineWidth: 1 * units.scale
    property bool fill: false
    property bool stroke: true
    property real alpha: 1.0

    onLineWidthChanged: requestPaint()
    onFillChanged: requestPaint()
    onStrokeChanged: requestPaint()
    onStrokeStyleChanged: requestPaint()
    onFillStyleChanged: requestPaint()

    onPaint: {
        var ctx = getContext("2d")
        var delta = 0

        ctx.save()
        ctx.clearRect(0, 0, triangle.width, triangle.height)
        ctx.strokeStyle = triangle.strokeStyle
        ctx.lineWidth = triangle.lineWidth
        ctx.fillStyle = triangle.fillStyle
        ctx.globalAlpha = triangle.alpha
        ctx.lineJoin = "round"
        ctx.beginPath()

        // Translate to middle.
        ctx.translate((0.5 * width - 0.5 * triangleWidth),
                      (0.5 * height - 0.5 * triangleHeight))

        // Draw the triangle.
        ctx.moveTo(0 + delta, 0) // left point of triangle
        ctx.lineTo(triangleWidth + delta, 0)
        ctx.lineTo(triangleWidth / 2 + delta, triangleHeight)

        ctx.closePath()

        if (triangle.fill) {
            ctx.fill()
        }

        if (triangle.stroke) {
            ctx.stroke()
        }

        ctx.restore()
    }
}
