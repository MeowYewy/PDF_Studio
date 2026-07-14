import QtQuick
import QtQuick.Controls
import ProjectO

ComboBox {
    id: control

    implicitHeight: 36
    implicitWidth: 100

    palette.window: Theme.surface
    palette.windowText: Theme.text
    palette.text: Theme.text
    palette.button: Theme.surface
    palette.buttonText: Theme.text
    palette.highlight: Theme.accent
    palette.highlightedText: "#FFFFFF"
    palette.base: Theme.surface
    palette.alternateBase: Theme.surfaceAlt

    background: Rectangle {
        radius: Theme.radiusSm
        color: Theme.surface
        border.color: control.activeFocus || control.popup.visible ? Theme.accent : Theme.border
        border.width: control.activeFocus || control.popup.visible ? 2 : 1
    }

    contentItem: Text {
        leftPadding: 12
        rightPadding: control.indicator.width + 12
        text: control.displayText
        font.pixelSize: Theme.mainFont.pixelSize
        font.family: Theme.mainFont.family
        font.weight: Font.DemiBold
        color: Theme.text
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    delegate: ItemDelegate {
        id: del
        width: control.width
        height: 34
        required property var modelData
        required property int index
        text: control.textRole ? (Array.isArray(control.model) && control.model[index] && control.model[index][control.textRole] !== undefined
              ? String(control.model[index][control.textRole])
              : String(modelData)) : String(modelData)
        palette.text: del.highlighted ? "#FFFFFF" : Theme.text
        palette.highlight: Theme.accent
        palette.highlightedText: "#FFFFFF"
        font.weight: del.highlighted ? Font.DemiBold : Font.Medium
        background: Rectangle {
            radius: 4
            color: del.highlighted ? Theme.accent
                   : (del.hovered ? Theme.menuHover : "transparent")
        }
    }

    popup: Popup {
        y: control.height + 4
        width: Math.max(control.width, 100)
        padding: 6
        modal: false
        dim: false

        background: Rectangle {
            radius: Theme.radiusSm
            color: Theme.surface
            border.color: Theme.border
            border.width: 1
        }

        contentItem: ListView {
            clip: true
            implicitHeight: Math.min(contentHeight, 180)
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            spacing: 2
            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }
}
