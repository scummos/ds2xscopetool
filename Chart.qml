import CustomComponents 1.0
import Qt 4.7

Rectangle {
    id: canvas
    width: 800
    height: 480
    color: "#111111"
    property color textColor: "white"
    property int fontSize: 6

    // Some nice lines
    Repeater {
        id: horizontalLines
        anchors.fill: parent
        model: 14
        Rectangle {
            width: 1
            height: canvas.height
            color: { index == 7 ? "#77CCCCCC" : "#44CCCCCC" }
            x: canvas.width / 14 * index
            y: 0
        }
    }
    Repeater {
        id: verticalLines
        anchors.fill: parent
        model: 10
        Rectangle {
            width: canvas.width
            height: 1
            color: { index == 5 ? "#77CCCCCC" : "#44CCCCCC" }
            x: 0
            y: canvas.height / 10 * index
        }
    }

    Text {
        id: horizontalMin
        color: parent.textColor
        font.pointSize: parent.fontSize
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: font.pixelSize * 1.5
//         text: { Math.round(parent.curves[0].getHorizontalMin(), 2) }
    }
    Text {
        id: horizontalMax
        color: parent.textColor
        font.pointSize: parent.fontSize
        anchors.right: parent.right
        anchors.bottom: parent.bottom
//         text: { Math.round(parent.curves[0].getHorizontalMin() + parent.curves[0].getHorizontalSpan()) }
    }
    Text {
        id: verticalMin
        color: parent.textColor
        font.pointSize: parent.fontSize
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.bottomMargin: font.pixelSize*1.5
//         text: { Math.round(parent.curves[0].getVerticalMin()) }
    }
    Text {
        id: verticalMax
        color: parent.textColor
        font.pointSize: parent.fontSize
        anchors.top: parent.top
        anchors.left: parent.left
//         text: { Math.round(parent.curves[0].getVerticalMin() + parent.curves[0].getVerticalSpan()) }
    }
}