import QtQuick
import QtQuick.Controls
import PageCase

ToolButton {
    id: control

    implicitWidth: 24
    implicitHeight: 24
    text: "×"
    font.pixelSize: 14
    palette.buttonText: Theme.textSecondary
    hoverEnabled: true

    background: Rectangle {
        radius: 4
        color: control.hovered ? Theme.menuHover : "transparent"
    }
}
