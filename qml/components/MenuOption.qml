import QtQuick
import QtQuick.Controls
import PageCase

Rectangle {
    id: option
    width: parent ? parent.width - 8 : 252
    anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
    height: 36
    radius: Theme.radiusSm

    property string label: ""
    property bool selected: false
    property bool hovered: false
    property string fontFamily: ""
    property int fontPixelSize: 0

    signal triggered()

    color: selected ? Theme.menuSelectedBg
         : (hovered ? Theme.menuHover : "transparent")

    Behavior on color { ColorAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic } }

    Text {
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        text: option.label
        font.pixelSize: option.fontPixelSize > 0 ? option.fontPixelSize : Theme.mainFont.pixelSize
        font.family: option.fontFamily || Theme.mainFont.family
        font.weight: option.selected ? Font.DemiBold : Font.Medium
        color: option.selected ? Theme.menuTextSelected : Theme.menuUnselectedText
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true
        onClicked: option.triggered()
        onEntered: option.hovered = true
        onExited: option.hovered = false
    }
}
