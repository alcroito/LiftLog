import QtQuick 2.2
import QtQuick.Controls 1.2

StackViewDelegate {
    id: root

    function getTransition(properties)
    {
        var enterItemTransitionOrientatin = properties.enterItem.transitionOrientation
        var exitItemTransitionOrientation = properties.exitItem.transitionOrientation
        return root[enterItemTransitionOrientatin === "vertical" || exitItemTransitionOrientation === "vertical" ? "verticalSlide" : "horizontalSlide"][properties.name]
    }

    function transitionFinished(properties)
    {
        properties.exitItem.x = 0
        properties.exitItem.y = 0
    }

    property QtObject horizontalSlide: QtObject {
        property Component pushTransition: StackViewTransition {
            PropertyAnimation {
                target: enterItem
                property: "x"
                from: target.width
                to: 0
                duration: 400
                easing.type: Easing.OutCubic
            }
            PropertyAnimation {
                target: exitItem
                property: "x"
                from: 0
                to: -target.width
                duration: 400
                easing.type: Easing.OutCubic
            }
        }

        property Component popTransition: StackViewTransition {
            PropertyAnimation {
                target: enterItem
                property: "x"
                from: -target.width
                to: 0
                duration: 400
                easing.type: Easing.OutCubic
            }
            PropertyAnimation {
                target: exitItem
                property: "x"
                from: 0
                to: target.width
                duration: 400
                easing.type: Easing.OutCubic
            }
        }
        property Component replaceTransition: pushTransition
    }

    property QtObject verticalSlide: QtObject {
        property Component pushTransition: StackViewTransition {
            PropertyAnimation {
                target: enterItem
                property: "y"
                from: target.height
                to: 0
                duration: 300
                easing.type: Easing.InQuad
            }
//            PropertyAnimation {
//                target: exitItem
//                property: "y"
//                from: 0
//                to: -target.height
//                duration: 300
//            }
        }

        property Component popTransition: StackViewTransition {
//            PropertyAnimation {
//                target: enterItem
//                property: "y"
//                from: -target.height
//                to: 0
//                duration: 300
//            }
            PropertyAnimation {
                target: exitItem
                property: "y"
                from: 0
                to: target.height
                duration: 300
                easing.type: Easing.InQuad
            }
            property Component replaceTransition: pushTransition
        }
    }
}
