import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

GridLayout {
    id: grid
    columns: 3
    width: 319

    Text { text: "Three"; font.bold: true; }
    Text { text: "words"; color: "red" }
    Text { text: "in"; font.underline: true }
    Text { text: "a"; font.pixelSize: 20 }
    Text { text: "row"; font.strikeout: true }
}
