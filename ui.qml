import CustomComponents 1.0
import Qt 4.7

import QtQuick 1.0

Rectangle {
    id: canvas
    anchors.fill: parent;
    color: "#111111"

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
        if (event.key == Qt.Key_M) {
            if ( menu.state == "NotVisibleState" ) {
                menu.state = "VisibleState";
            }
            else {
                menu.state = "NotVisibleState";
            }
            event.accepted = true;
        }
     }

    Chart {
        objectName: "mychart"
    }

    Rectangle {
        id: menu
        anchors.right: parent.right
        y: 15
        Keys.onDownPressed: menuButtonList.incrementCurrentIndex()
        Keys.onUpPressed: menuButtonList.decrementCurrentIndex()
        Keys.onLeftPressed: menuButtonList.currentItem.selected -= 1
        Keys.onRightPressed: menuButtonList.currentItem.selected += 1
        states: [
            State {
                name: "VisibleState"
                PropertyChanges { target: menu; width: 120+15 }
                PropertyChanges { target: menu; opacity: 1 }
                PropertyChanges { target: menu; focus: true }
                PropertyChanges { target: canvas; focus: false }
            },
            State {
                name: "NotVisibleState"
                PropertyChanges { target: menu; width: 0 }
                PropertyChanges { target: menu; opacity: 0 }
                PropertyChanges { target: menu; focus: false }
                PropertyChanges { target: canvas; focus: true }
            }
        ]
        ListView {
            id: menuButtonList
            anchors.margins: 50
            height: canvas.height
            onCurrentItemChanged: {
                console.log(currentIndex); console.log(currentItem)
                console.log(currentItem.ListView.isCurrentItem)
            }
            model: ListModel {
                ListElement {
                    text: "Acq: %r"
                    toggleValues: [ ListElement { value: "Full" } , ListElement { value: "Displayed" } ]
                    notifyParamName: "AcquisitionMode"
                }
                ListElement {
                    text: "AcqTime: %rms"
                    toggleValues: [ ListElement { value: 50 }, ListElement { value: 100 }, ListElement { value: 300 },
                                     ListElement { value: 1000 }, ListElement { value: 2000 }, ListElement { value: 20 }
                    ]
                    notifyParamName: "AcquisitionTime"
                }
            }
            delegate: Rectangle {
                id: wrapper
                state: ListView.isCurrentItem ? "SelectedState" : "NormalState"
                onStateChanged: button.state = state
                property int selected: 0
                MenuButton {
                    id: button
                    buttonText: {
                        if ( parent.selected >= toggleValues.count ) {
                            parent.selected = 0;
                        }
                        if ( parent.selected < 0 ) {
                            parent.selected = toggleValues.count - 1;
                        }
                        text.replace("%r", toggleValues.get(parent.selected).value);
                    }
                }
                height: button.height + 10 // controls the spacing between the buttons
            }
        }
        Behavior on width { NumberAnimation { duration: 120 } }
        Behavior on opacity { NumberAnimation { duration: 120 } }
        Component.onCompleted: state = "NotVisibleState"
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

        Component.onCompleted: state = "NotVisibleState";

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
}
