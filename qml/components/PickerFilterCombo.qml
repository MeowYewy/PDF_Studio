import QtQuick
import QtQuick.Controls
import PageCase

Item {
    id: root

    implicitWidth: 156
    implicitHeight: 38

    property var model: []
    property int currentIndex: 0
    signal activated(int index)

    readonly property int optionCount: model ? model.length : 0
    readonly property string displayText: {
        if (!model || currentIndex < 0 || currentIndex >= model.length)
            return ""
        const item = model[currentIndex]
        return item && item.label !== undefined ? String(item.label) : ""
    }

    function repositionPopup() {
        const topLeft = root.mapToItem(Overlay.overlay, 0, 0)
        menuPopup.x = topLeft.x
        menuPopup.width = root.width
        menuPopup.y = topLeft.y - menuPopup.height - 4
    }

    WheelHandler {
        onWheel: function(event) {
            event.accepted = true
        }
    }

    Rectangle {
        anchors.fill: parent
        radius: Theme.radiusSm
        color: comboMouse.containsMouse || menuPopup.visible ? Theme.menuHover : Theme.surfaceAlt
        border.color: menuPopup.visible ? Theme.accent : Theme.border
        border.width: menuPopup.visible ? 2 : 1
        clip: true

        Behavior on color {
            ColorAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic }
        }

        MouseArea {
            id: comboMouse
            anchors.fill: parent
            hoverEnabled: true
            onWheel: function(wheel) {
                wheel.accepted = true
            }
            onClicked: {
                if (menuPopup.visible) {
                    menuPopup.close()
                } else {
                    menuPopup.open()
                    Qt.callLater(root.repositionPopup)
                }
            }
        }

        Text {
            anchors.fill: parent
            leftPadding: 12
            rightPadding: 24
            text: root.displayText
            font.pixelSize: 15
            font.family: Theme.mainFont.family
            font.weight: Font.Normal
            color: Theme.text
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        Text {
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            text: "▴"
            font.pixelSize: 14
            color: Theme.text
        }
    }

    Popup {
        id: menuPopup
        parent: Overlay.overlay
        z: 7000
        width: root.width
        padding: 6
        modal: false
        dim: false
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        onOpened: Qt.callLater(root.repositionPopup)

        background: Rectangle {
            radius: Theme.radiusSm
            color: Theme.surface
            border.color: Theme.border
            border.width: 1
        }

        contentItem: ListView {
            clip: true
            spacing: 2
            interactive: false
            implicitHeight: 36 * root.optionCount + Math.max(0, root.optionCount - 1) * 2
            model: root.model
            boundsBehavior: Flickable.StopAtBounds
            onContentHeightChanged: if (menuPopup.visible)
                                        Qt.callLater(root.repositionPopup)

            delegate: ItemDelegate {
                id: optionDelegate
                width: ListView.view.width
                height: 36
                leftPadding: 0
                rightPadding: 0
                topPadding: 0
                bottomPadding: 0
                spacing: 0
                required property var modelData
                required property int index
                readonly property bool picked: index === root.currentIndex
                text: modelData && modelData.label !== undefined ? String(modelData.label) : ""
                font.pixelSize: 15
                font.family: Theme.mainFont.family
                font.weight: Font.Normal
                contentItem: Text {
                    width: parent.width
                    height: parent.height
                    text: optionDelegate.text
                    font: optionDelegate.font
                    color: optionDelegate.picked ? "#FFFFFF" : Theme.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    radius: 4
                    anchors.fill: parent
                    color: optionDelegate.picked ? Theme.accent
                           : (optionDelegate.hovered ? Theme.menuHover : "transparent")
                }
                onClicked: {
                    root.currentIndex = index
                    root.activated(index)
                    menuPopup.close()
                }
            }
        }
    }
}
