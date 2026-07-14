import QtQuick
import QtQuick.Controls
import ProjectO

Button {
    id: control

    implicitHeight: 36
    padding: 10
    hoverEnabled: true

    scale: control.down ? 0.97 : 1.0
    Behavior on scale {
        NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic }
    }

    palette.button: highlighted ? Theme.accent : Theme.surfaceAlt
    palette.buttonText: highlighted ? "#FFFFFF" : Theme.text

    background: Rectangle {
        radius: Theme.radiusSm
        color: control.down ? Theme.tabInactive
               : (control.hovered
                  ? (control.highlighted ? Theme.accentLight : Theme.menuHover)
                  : (control.highlighted ? Theme.accent : Theme.surfaceAlt))
        border.color: control.highlighted ? Theme.accent : Theme.border
        border.width: 1

        Behavior on color {
            ColorAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic }
        }
    }

    contentItem: Text {
        text: control.text
        font: control.highlighted ? Theme.mainFontBold : Theme.mainFont
        color: control.highlighted ? "#FFFFFF" : Theme.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
