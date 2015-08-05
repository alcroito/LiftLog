import QtQuick 2.0

MouseArea {
    id: root
    width: 20

    property int startX
    property int startY
    property int bigDelta: 40
    property int smallDelta: 40
    property bool handled: false
    signal swipeLeft
    signal swipeRight
    signal swipeUp
    signal swipeDown
    propagateComposedEvents: true

    onPressed: {
        startX = mouse.x
        startY = mouse.y
    }

    onPositionChanged: {
        var deltax = mouse.x - startX;
        var deltay = mouse.y - startY;

        if (Math.abs(deltax) > bigDelta || Math.abs(deltay) > bigDelta) {
            if (deltax > smallDelta && Math.abs(deltay) < smallDelta && !handled) {
                swipeRight()
                handled = true
            } else if (deltax < -smallDelta && Math.abs(deltay) < smallDelta && !handled) {
                swipeLeft()
                handled = true
            } else if (Math.abs(deltax) < smallDelta && deltay > smallDelta && !handled) {
                swipeDown()
                handled = true
            } else if (Math.abs(deltax) < smallDelta && deltay < smallDelta & !handled) {
                swipeUp()
                handled = true
            }
        }
    }

    onReleased: {
        handled = false
    }
}
