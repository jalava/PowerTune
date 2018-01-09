import QtQuick 2.0
import QtQuick.Controls 2.1


Rectangle {
    property  int position: Dashboard.mapPos
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
                id: rect
                width: parent.width/25; height: parent.height /25
                color: "lightsteelblue"
                border.color: "black"

                states: State {
                    name: "active"; when: position.valueOf() >= 1
                    PropertyChanges { target: repeat.itemAt(position.valueOf()).color = "blue"}
                }
                states: State {
                    name: "notactive"; when: position.valueOf() < 1
                    PropertyChanges { target: repeat.itemAt(position.valueOf()).color = "lightsteelblue"}
                }

            }
        }
        //
    }
}
