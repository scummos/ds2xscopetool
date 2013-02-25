import CustomComponents 1.0
import Qt 4.7

import QtQuick 1.0

Rectangle {
    id: canvas
    anchors.fill: parent;
    color: "#111111"

    MouseArea {
        id: dataRangeManager
        objectName: "dataRangeManager"
        signal dataRangeChangeRequested(string channel, string axis, string kind, int newX, int newY);
        signal dataRangeChangeCompleted();
        property string operation: "undefined";
        property string channel: "undefined";
        property string axis: "undefined";
        anchors.fill: parent
        hoverEnabled: false
        onPositionChanged: dataRangeChangeRequested(channel, axis, operation, mouse.x, mouse.y);
        onClicked: { state = "InactiveState"; dataRangeChangeCompleted(); }
        states: [
            State {
                name: "InactiveState"
                PropertyChanges { target: dataRangeManager; hoverEnabled: false }
                PropertyChanges { target: notification; state: "InvisibleState" }
            },
            State {
                name: "SelectChannelState";
                PropertyChanges { target: notification; notificationText: "Select channel" }
                PropertyChanges { target: notification; state: "VisibleState" }
            },
            State {
                name: "SelectAxisState";
                PropertyChanges { target: notification; notificationText: "Select axis" }
                PropertyChanges { target: notification; state: "VisibleState" }
            },
            // TODO could merge those two
            State {
                name: "MoveChannelState"
                PropertyChanges { target: dataRangeManager; hoverEnabled: true }
                PropertyChanges { target: dataRangeManager; operation: "move" }
            },
            State {
                name: "ScaleChannelState"
                PropertyChanges { target: dataRangeManager; hoverEnabled: true }
                PropertyChanges { target: notification; notificationText: "Scale channel" }
                PropertyChanges { target: notification; state: "VisibleState" }
                PropertyChanges { target: dataRangeManager; operation: "scale" }
            }
        ]
        Component.onCompleted: state = "InactiveState"
    }

    Rectangle {
        id: notification
        property string notificationText: "";
        color: "#22FFFFFF";
        z: 1000
        anchors.centerIn: parent
        clip: true
        opacity: 0.0
        width: 130
        height: 20
        Behavior on opacity { NumberAnimation { duration: 80 } }
        Behavior on width { NumberAnimation { duration: 150 } }
        Behavior on height { NumberAnimation { duration: 150 } }
        onNotificationTextChanged: notificationText.text = notification.notificationText

        Text {
            id: notificationText
            Behavior on opacity {
                NumberAnimation { duration: 150 }
            }
            opacity: parent.opacity
            anchors.fill: parent
            color: "#CCCCCC"
            font.family: "monospace"
            font.pointSize: 11
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        states: [
            State {
                name: "InvisbleState"
            },
            State {
                name: "VisibleState"
                PropertyChanges { target: notificationText; restoreEntryValues: false; explicit: true;
                                  text: notification.notificationText }
                PropertyChanges { target: notification; opacity: 1.0 }
                PropertyChanges { target: notification; width: 300 }
                PropertyChanges { target: notification; height: 40 }
            }
        ]

        Component.onCompleted: state = "InvisibleState"
    }

    Keys.onPressed: {
        if ( event.key == Qt.Key_F1 ) {
            if ( textEditor.state == "NotVisibleState" ) {
                textEditor.state = "VisibleState";
            }
            else {
                textEditor.state = "NotVisibleState";
            }
            event.accepted = true;
        }
        if ( event.key == Qt.Key_M ) {
            if ( menu.state == "NotVisibleState" ) {
                menu.state = "VisibleState";
            }
            else {
                menu.state = "NotVisibleState";
            }
            event.accepted = true;
        }
        if ( dataRangeManager.state == "InactiveState" ) {
            var operations = Object();
            operations[Qt.Key_G] = "MoveChannelState";
            operations[Qt.Key_S] = "ScaleChannelState";
            if ( operations[event.key] != undefined ) {
                dataRangeManager.operation = operations[event.key];
                dataRangeManager.state = "SelectChannelState";
            }
        }
        else if ( dataRangeManager.state == "SelectChannelState" ) {
            var channels = Object();
            channels[Qt.Key_1] = "Channel1";
            channels[Qt.Key_2] = "Channel2";
            channels[Qt.Key_3] = "jsMathLine";
            channels[Qt.Key_4] = "fixedMathLine";
            if ( channels[event.key] != undefined ) {
                dataRangeManager.channel = channels[event.key];
                dataRangeManager.state = "SelectAxisState";
            }
        }
        else if ( dataRangeManager.state == "SelectAxisState" ) {
            var axes = Object();
            axes[Qt.Key_X] = "x";
            axes[Qt.Key_Y] = "y";
            if ( axes[event.key] != undefined ) {
                dataRangeManager.axis = axes[event.key];
                dataRangeManager.state = dataRangeManager.operation;
            }
        }
        if ( dataRangeManager.state != "InactiveState" ) {
            if ( event.key == Qt.Key_Escape ) {
                dataRangeManager.state = "InactiveState";
            }
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
            objectName: "menuButtonList"
            signal paramChanged(string name, variant newValue)
            anchors.margins: 50
            height: canvas.height
            onCurrentItemChanged: {
                console.log(currentIndex); console.log(currentItem)
                console.log(currentItem.ListView.isCurrentItem)
            }
            model: ListModel {
                id: menuSettings
                ListElement {
                    text: "Acq: %r"
                    toggleValues: [ ListElement { value: "Full" }, ListElement { value: "Displayed" } ]
                    notifyParamName: "AcquisitionMode"
                }
                ListElement {
                    text: "AcqTime: %rms"
                    toggleValues: [ ListElement { value: 50 }, ListElement { value: 100 }, ListElement { value: 300 },
                                     ListElement { value: 1000 }, ListElement { value: 2000 }, ListElement { value: 20 }
                    ]
                    notifyParamName: "AcquisitionTime"
                }
                ListElement {
                    text: "Update: %r"
                    toggleValues: [ ListElement { value: "Auto" }, ListElement { value: "Freeze" } ]
                    notifyParamName: "UpdateType"
                }
            }
            delegate: Rectangle {
                id: wrapper
                state: ListView.isCurrentItem ? "SelectedState" : "NormalState"
                onStateChanged: button.state = state
                property int selected: 0
                ListView {
                    rotation: 180
                    width: 300
                    x: -300
                    opacity: wrapper.ListView.isCurrentItem ? 0.8 : 0.2
                    Behavior on opacity { NumberAnimation { duration: 100 } }
                    z: 30
                    model: toggleValues
                    orientation: ListView.Horizontal
                    delegate: Rectangle {
                        rotation: 180
                        width: text.width + 8
                        Text {
                            y: 5
                            horizontalAlignment: Text.AlignRight
                            id: text
                            property int reversedIndex: toggleValues.count - index - 1
                            text: toggleValues.get(reversedIndex).value
                            color: menuButtonList.currentItem.selected == (reversedIndex) && wrapper.ListView.isCurrentItem ? "#FF8400" : "white"
                            Behavior on color { ColorAnimation { duration: 100 } }
                        }
                    }
                }
                onSelectedChanged: menuButtonList.paramChanged(notifyParamName, toggleValues.get(wrapper.selected).value)
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
                text: "y1+y2+5000"
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
