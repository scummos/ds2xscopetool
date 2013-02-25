import QtQuick 1.0

Item {
    id: root
    property string buttonFor
    property string value
    property color color
    property string displayName
    onValueChanged: {
        if ( value == "Off" ) {
            root.opacity = 0.4
        }
        else {
            root.opacity = 1.0
        }
    }
    Behavior on opacity { NumberAnimation { duration: 60 } }

    width: 21 + 14 + text.width
    height: 21

    ListView {
        z: 5
        id: sinewave
        anchors.fill: parent
        anchors.topMargin: 5
        orientation: ListView.Horizontal
        model: ListModel {
            id: sine
            Component.onCompleted: {
                for ( var i = 0; i < 21; i ++ ) {
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
        anchors.fill: sinewave
        horizontalAlignment: Text.AlignCenter
        verticalAlignment: Text.AlignVCenter
        font.family: "monospace"
        font.pointSize: 5
        anchors.leftMargin: 3.5
        anchors.topMargin: 6.5
        z: 10
        color: "white"
        text: root.displayName
        opacity: 0.6
    }

    Text {
        id: text
        text: root.value
        anchors.left: root.left
        anchors.leftMargin: 25
        y: 5
        font.family: "monospace"
        color: "white"
        horizontalAlignment: Text.AlignLeft
    }
}