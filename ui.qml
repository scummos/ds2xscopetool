import CustomComponents 1.0
import Qt 4.7

import QtQuick 1.0

Rectangle {
    id: canvas
    anchors.fill: parent;
    color: "#333333"

    Keys.onPressed: {
        if (event.key == Qt.Key_F1) {
            if ( textEditor.state == "NotVisibleState" ) {
                textEditor.state = "VisibleState";
            }
            else {
                textEditor.state = "NotVisibleState";
            }
            event.accepted = true;
        }
     }

    Chart {
        objectName: "mychart"
    }

    Rectangle {
        id: textEditor

        states: [
            State {
                name: "NotVisibleState"
                PropertyChanges { target: textEditor; x: -500 }
                PropertyChanges { target: jsChannel; focus: false }
                PropertyChanges { target: canvas; focus: true }
            },
            State {
                name: "VisibleState"
                PropertyChanges { target: textEditor; x: 15 }
                PropertyChanges { target: jsChannel; focus: true }
            }
        ]

        Behavior on x { NumberAnimation { duration: 200 } }
        z: 10
        color: Qt.rgba(0.2, 0.2, 0.2, 0.8)
        width: 240
        height: 100
        radius: 3
        x: 15
        y: 15
        border.color: "#777777"
        border.width: 1

        Flickable {
            id: flick

            clip: true
            anchors.fill: parent

            function ensureVisible(r)
            {
                if (contentX >= r.x)
                    contentX = r.x;
                else if (contentX+width <= r.x+r.width)
                    contentX = r.x+r.width-width;
                if (contentY >= r.y)
                    contentY = r.y;
                else if (contentY+height <= r.y+r.height)
                    contentY = r.y+r.height-height+25;
            }

            TextEdit {
                function doCalculation(y1, y2, code) {
                    return eval(code);
                }

                id: jsChannel
                objectName: "jsChannel"
                anchors.leftMargin: 8
                anchors.topMargin: 8
                anchors.fill: parent
                text: "y1+y2"
                font.family: "Monospace"
                font.pointSize: 8
                color: "#CCCCCC"
                textFormat: TextEdit.PlainText
                focus: true
                width: flick.width
                height: flick.height
                wrapMode: TextEdit.Wrap
                onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
            }
        }
    }

    Component.onCompleted: state = "defaultState";

}
