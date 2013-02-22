import CustomComponents 1.0
import Qt 4.7

Rectangle {
    property bool evenClick : false

    anchors.fill: parent; color: "#333333"

    Chart {
        id: mychart
    }

//     PlotLine {
//         id: diagonalLine
//
//         viewport {
//             width: 100
//             height: 100
//             x: 20
//             y: 20
//         }
//
//         color: "red"; penWidth: 1; smooth: true
//     }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            diagonalLine.viewport.width += 20;
        }
    }

    Text {
        anchors.left: parent.left; anchors.top: parent.top
        text: "x1: " + diagonalLine.viewport.x + " y1: " + diagonalLine.viewport.y
    }
}
