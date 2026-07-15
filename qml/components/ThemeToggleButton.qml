import QtQuick
import PageCase

Rectangle {
    id: btn
    property string label: ""
    property bool selected: false
    property bool hovered: false
    signal clicked()

    radius: Theme.radiusSm - 2

    color: selected ? Theme.menuSelectedBg
         : (hovered ? Theme.menuHover : "transparent")

    border.width: 0

    Behavior on color { ColorAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }

    Text {
        anchors.centerIn: parent
        text: btn.label
        font.pixelSize: Theme.mainFont.pixelSize
        font.family: Theme.mainFont.family
        font.weight: btn.selected ? Font.DemiBold : Font.Medium
        color: btn.selected ? Theme.menuTextSelected : Theme.menuUnselectedText
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true
        onClicked: btn.clicked()
        onEntered: btn.hovered = true
        onExited: btn.hovered = false
    }
}
