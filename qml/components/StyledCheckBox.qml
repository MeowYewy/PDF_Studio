import QtQuick
import QtQuick.Layouts
import PageCase

RowLayout {
    id: root

    property bool checked: false
    property string text: ""
    property bool enabled: true

    signal toggled(bool checked)

    spacing: 8
    opacity: enabled ? 1 : 0.45

    readonly property int boxSize: 17
    readonly property int boxRadius: 4

    Item {
        Layout.preferredWidth: root.boxSize
        Layout.preferredHeight: root.boxSize
        Layout.alignment: Qt.AlignVCenter
        Layout.topMargin: 1

        Rectangle {
            id: box
            anchors.fill: parent
            radius: root.boxRadius
            color: root.checked ? Theme.accent : Theme.surfaceAlt
            border.color: root.checked ? Theme.accent
                                    : (mouseArea.containsMouse ? Theme.accentLight : Theme.border)
            border.width: 1

            Behavior on color {
                ColorAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic }
            }
            Behavior on border.color {
                ColorAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic }
            }
        }

        Canvas {
            id: checkMark
            anchors.fill: parent
            visible: root.checked

            onVisibleChanged: if (visible) requestPaint()
            onWidthChanged: requestPaint()
            onHeightChanged: requestPaint()

            onPaint: {
                const ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                ctx.strokeStyle = "#FFFFFF"
                ctx.lineWidth = 2
                ctx.lineCap = "round"
                ctx.lineJoin = "round"
                ctx.beginPath()
                const s = root.boxSize
                ctx.moveTo(s * 0.22, s * 0.52)
                ctx.lineTo(s * 0.42, s * 0.72)
                ctx.lineTo(s * 0.78, s * 0.28)
                ctx.stroke()
            }
        }
    }

    Text {
        Layout.alignment: Qt.AlignVCenter
        text: root.text
        font: Theme.mainFont
        color: Theme.text
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: root.enabled
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.toggled(!root.checked)
    }
}
