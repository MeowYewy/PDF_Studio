import QtQuick
import QtQuick.Controls
import PageCase

ComboBox {
    id: control

    readonly property int comboFontSize: 15

    hoverEnabled: true
    wheelEnabled: false
    focusPolicy: Qt.ClickFocus

    WheelHandler {
        onWheel: function(event) {
            event.accepted = true
        }
    }

    indicator: Text {
        x: control.width - width - 10
        y: (control.height - height) / 2
        text: "▴"
        font.pixelSize: 14
        color: Theme.text
    }

    background: Rectangle {
        radius: Theme.radiusSm
        color: control.hovered || control.popup.visible ? Theme.menuHover : Theme.surfaceAlt
        border.color: control.popup.visible ? Theme.accent : Theme.border
        border.width: control.popup.visible ? 2 : 1
        clip: true

        Behavior on color {
            ColorAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            propagateComposedEvents: true
            onWheel: function(wheel) {
                wheel.accepted = true
            }
            onPressed: function(mouse) {
                mouse.accepted = false
            }
        }
    }

    onActivated: control.focus = false

    contentItem: Text {
        anchors.fill: parent
        leftPadding: control.indicator.width + 8
        rightPadding: control.indicator.width + 8
        text: control.displayText
        font.pixelSize: control.comboFontSize
        font.family: Theme.mainFont.family
        font.weight: Font.Normal
        color: Theme.text
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
    }

    delegate: ItemDelegate {
        id: del
        width: ListView.view ? ListView.view.width : control.width
        height: 36
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0
        spacing: 0
        required property var modelData
        text: modelData && modelData.label !== undefined ? modelData.label : ""
        palette.text: del.highlighted ? "#FFFFFF" : Theme.text
        palette.highlight: Theme.accent
        palette.highlightedText: "#FFFFFF"
        font.pixelSize: control.comboFontSize
        font.family: Theme.mainFont.family
        font.weight: Font.Normal
        contentItem: Text {
            width: parent.width
            height: parent.height
            text: del.text
            font: del.font
            color: del.highlighted ? "#FFFFFF" : Theme.text
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        background: Rectangle {
            radius: 4
            anchors.fill: parent
            color: del.highlighted ? Theme.accent
                   : (del.hovered ? Theme.menuHover : "transparent")
        }
    }

    popup: Popup {
        id: comboPopup
        width: control.width
        padding: 6
        modal: false
        dim: false

        onOpened: Qt.callLater(function() {
            comboPopup.y = -comboPopup.height - 4
        })

        onClosed: control.focus = false

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
            onContentHeightChanged: if (comboPopup.visible)
                                        Qt.callLater(function() {
                                            comboPopup.y = -comboPopup.height - 4
                                        })
        }
    }
}
