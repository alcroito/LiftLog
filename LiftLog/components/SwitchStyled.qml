import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import "../UI.js" as UI

Switch {
    id: root
    property color backgroundColorChecked: "#e64c3c"
    property color backgroundColorUnchecked: "#ffffff"
    property color backgroundBorderColorChecked: "#e64c3c"
    property color backgroundBorderColorUnchecked: "#dadada"

    Component {
        id: ios
        SwitchStyleIOS {
            backgroundColorChecked: root.backgroundColorChecked
            backgroundColorUnchecked: root.backgroundColorUnchecked
            backgroundBorderColorChecked: root.backgroundBorderColorChecked
            backgroundBorderColorUnchecked: root.backgroundBorderColorUnchecked
        }
    }

    Component {
        id: standard
        SwitchStyle {}
    }

    style: {
        if (UI.style == "osx") { style = ios } else { style = standard }
    }
}

