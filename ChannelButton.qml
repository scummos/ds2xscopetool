import QtQuick 1.0

Item {
    id: root
    property string buttonFor
    property string value
    property color color
    onValueChanged: {
        if ( value == "Off" ) {
            root.opacity = 0.4
        }
        else {
            root.opacity = 1.0
        }
    }
    Behavior on opacity { NumberAnimation { duration: 60 } }

    width: 20 + 14 + text.width
    height: 20

    ListView {
        id: sinewave
        anchors.fill: parent
        anchors.topMargin: 5
        orientation: ListView.Horizontal
        model: ListModel {
            id: sine
            Component.onCompleted: {
                for ( var i = 0; i < 20; i ++ ) {
                    sine.append({value: -Math.sin(i/(20/(2*3.14)))*3.5 + 7})
                }
            }
        }
        delegate: Rectangle {
            color: root.color
            width: 1
            height: 1
            y: value
        }
    }

    Text {
        id: text
        text: root.value
        anchors.left: root.left
        anchors.leftMargin: 25
        y: 4
        font.family: "monospace"
        color: "white"
        horizontalAlignment: Text.AlignLeft
    }
}