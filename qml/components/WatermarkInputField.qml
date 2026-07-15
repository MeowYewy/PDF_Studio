import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import PageCase

Item {
    id: root

    implicitHeight: 38
    implicitWidth: 200

    property string text: ""
    property string placeholderText: ""
    signal textEdited(string value)

    function anchorHistoryPopup() {
        const topLeft = root.mapToItem(Overlay.overlay, 0, 0)
        historyPopup.x = topLeft.x
        historyPopup.width = Math.max(root.width, 220)
        historyPopup.y = topLeft.y - historyPopup.height - 4
    }

    function openHistoryPopup() {
        const topLeft = root.mapToItem(Overlay.overlay, 0, 0)
        historyPopup.x = topLeft.x
        historyPopup.width = Math.max(root.width, 220)
        historyPopup.open()
        Qt.callLater(root.anchorHistoryPopup)
    }

    TextField {
        id: field
        anchors.left: parent.left
        anchors.right: historyBtn.left
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        height: 38
        placeholderText: root.placeholderText
        text: root.text
        font: Theme.mainFont
        color: Theme.text
        placeholderTextColor: Theme.textSecondary
        maximumLength: 128
        onTextChanged: {
            root.text = text
            root.textEdited(text)
        }
        background: Rectangle {
            radius: Theme.radiusSm
            color: Theme.surfaceAlt
            border.color: field.activeFocus ? Theme.accent : Theme.border
            border.width: field.activeFocus ? 2 : 1
        }
    }

    ToolButton {
        id: historyBtn
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        width: 30
        height: 38
        text: "▴"
        font.pixelSize: 13
        palette.buttonText: Theme.textSecondary
        enabled: AppSettings.watermarkHistory.length > 0
        background: Rectangle {
            radius: Theme.radiusSm
            color: historyBtn.hovered ? Theme.menuHover : Theme.surfaceAlt
            border.color: historyPopup.visible ? Theme.accent : Theme.border
            border.width: 1
        }
        onClicked: root.openHistoryPopup()
    }

    Popup {
        id: historyPopup
        parent: Overlay.overlay
        padding: 6
        modal: false
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        z: 5000

        onOpened: Qt.callLater(root.anchorHistoryPopup)

        onClosed: historyBtn.focus = false

        background: Rectangle {
            radius: Theme.radiusSm
            color: Theme.surface
            border.color: Theme.border
            border.width: 1
        }

        contentItem: ListView {
            id: historyList
            clip: true
            implicitHeight: Math.min(contentHeight, 132)
            spacing: 2
            model: AppSettings.watermarkHistory

            onContentHeightChanged: if (historyPopup.visible)
                                        Qt.callLater(root.anchorHistoryPopup)

            delegate: Item {
                required property int index
                required property string modelData

                width: historyList.width
                height: 36

                Rectangle {
                    anchors.fill: parent
                    radius: 4
                    color: rowMouse.containsMouse ? Theme.menuHover : "transparent"
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 4
                    spacing: 6

                    Text {
                        Layout.fillWidth: true
                        text: modelData
                        elide: Text.ElideRight
                        font: Theme.mainFont
                        color: Theme.text
                    }

                    IconDeleteButton {
                        onClicked: AppSettings.removeWatermarkHistoryAt(index)
                    }
                }

                MouseArea {
                    id: rowMouse
                    anchors.fill: parent
                    anchors.rightMargin: 30
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        field.text = modelData
                        root.text = modelData
                        root.textEdited(modelData)
                        historyPopup.close()
                    }
                }
            }
        }
    }
}
