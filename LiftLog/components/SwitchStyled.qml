import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import "../UI.js" as UI

Switch {
    Component {
        id: ios
        SwitchStyleIOS {}
    }


    Component {
        id: standard
        SwitchStyle {}
    }

    style: {
        if (UI.style == "osx") { style = ios } else { style = standard }
    }
}

