import QtQuick
import QtQuick.Controls
import PageCase

Item {
    id: root

    implicitWidth: Theme.compactControlWidth
    implicitHeight: 38

    property int countValue: 1

    readonly property var countOptions: [
        { label: "1", value: 1 },
        { label: "2", value: 2 },
        { label: "3", value: 3 },
        { label: "4", value: 4 },
        { label: "5", value: 5 }
    ]

    property int selectedIndex: 0

    function applySelection(index) {
        if (index < 0 || index >= countOptions.length)
            return
        selectedIndex = index
        countValue = countOptions[index].value
    }

    WheelHandler {
        onWheel: function(event) {
            event.accepted = true
        }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        radius: Theme.radiusSm
        color: mouseArea.containsMouse || menuPopup.visible ? Theme.menuHover : Theme.surfaceAlt
        border.color: menuPopup.visible ? Theme.accent : Theme.border
        border.width: menuPopup.visible ? 2 : 1
        clip: true

        Behavior on color {
            ColorAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            onWheel: function(wheel) {
                wheel.accepted = true
            }
            onClicked: {
                if (menuPopup.visible)
                    menuPopup.close()
                else
                    menuPopup.open()
            }
        }

        Text {
            anchors.fill: parent
            leftPadding: 20
            rightPadding: 20
            text: countOptions[selectedIndex].label
            font.pixelSize: 15
            font.family: Theme.mainFont.family
            font.weight: Font.Normal
            color: Theme.text
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
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
        x: 0
        width: root.width
        padding: 6
        modal: false
        dim: false
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        onOpened: Qt.callLater(function() {
            menuPopup.y = -menuPopup.height - 4
        })

        background: Rectangle {
            radius: Theme.radiusSm
            color: Theme.surface
            border.color: Theme.border
            border.width: 1
        }

        contentItem: ListView {
            clip: true
            spacing: 2
            implicitHeight: 36 * countOptions.length + Math.max(0, countOptions.length - 1) * 2
            model: countOptions
            boundsBehavior: Flickable.StopAtBounds

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
                readonly property bool picked: index === root.selectedIndex
                text: modelData.label
                font.pixelSize: 15
                font.family: Theme.mainFont.family
                font.weight: Font.Normal
                palette.text: picked ? "#FFFFFF" : Theme.text
                palette.highlight: Theme.accent
                palette.highlightedText: "#FFFFFF"
                contentItem: Text {
                    width: parent.width
                    height: parent.height
                    text: optionDelegate.text
                    font: optionDelegate.font
                    color: optionDelegate.picked ? "#FFFFFF" : Theme.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    radius: 4
                    anchors.fill: parent
                    color: optionDelegate.picked ? Theme.accent
                           : (optionDelegate.hovered ? Theme.menuHover : "transparent")
                }
                onClicked: {
                    root.applySelection(index)
                    menuPopup.close()
                }
            }
        }
    }

    Component.onCompleted: applySelection(0)
}
