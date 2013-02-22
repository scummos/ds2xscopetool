import QtQuick 1.0
import CustomComponents 1.0
import Qt 4.7

PlotLine {
    id: line
    color: "blue"
    width: 50
    height: 50
    function setup() {
        anchors.fill = parent
    }
}