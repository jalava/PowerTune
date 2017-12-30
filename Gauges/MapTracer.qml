import QtQuick 2.0
import QtQuick.Controls 2.1

    Rectangle {
    id: root
    anchors.fill: parent
    Grid{
        width: root.width
        height: root.height
        anchors.bottom: parent.bottom
        columns: 20
        spacing: 1
        Repeater {
            id: repeat
            model: 400
            Rectangle {
                width: parent.width/25; height: parent.height /25
                color: "lightsteelblue"
                border.color: "black"

                // This is  just to test how to change the rectangles at certain positions in the grid has to be replaced by Map Position
                MouseArea {
                    anchors.fill: parent
                    onClicked: {

                        {repeat.itemAt(10).color = "red"}
                        {repeat.itemAt(5).color = "green"}
            }
        }


}
    }
//
}
}
