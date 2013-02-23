import QtQuick 1.0

Rectangle {
    id: background
    property string buttonText: "<no text>"
    border.width: 1
    border.color: "#666666"
    width: 120
    height: 25
    Behavior on color { ColorAnimation { duration: 200 } }
    states: [
        State {
            name: "SelectedState"
            PropertyChanges { target: background; color: "#FF8400" }
            PropertyChanges { target: text; color: "black" }
        },
        State {
            name: "NormalState"
            PropertyChanges { target: background; color: "#33AAAAAA" }
            PropertyChanges { target: text; color: "white" }
        }
    ]
    Text {
        id: text
        Behavior on color { ColorAnimation { duration: 100 } }
        text: parent.buttonText
        anchors.centerIn: parent
    }
    Component.onCompleted: state = "NormalState"
}