import QtQuick 2.0
import LiftLog 1.0

Item {
    id: cellTypeWrapper
    height: 55 * units.scale
    width: appState.windowWidth

    property var itemModelData
    property int itemModelIndex: -1
//            onItemModelDataChanged: {
//                console.log("data", itemModelData, itemModelIndex);
//            }
//            onItemModelIndexChanged: {
//                console.log("index", itemModelData, itemModelIndex);
//            }
}

