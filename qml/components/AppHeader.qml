import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ProjectO

Rectangle {
    id: header
    color: Theme.surface

    signal menuRequested(Item anchor)

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 1
        color: Theme.border
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 32
        anchors.rightMargin: 28
        spacing: 12

        AppLogo {
            Layout.alignment: Qt.AlignVCenter
            logoSize: 32
            cornerRadius: 8
        }

        Text {
            text: Theme.tr("appName")
            font: Theme.titleFont
            color: Theme.text
        }

        Item { Layout.fillWidth: true }

        ToolButton {
            id: menuBtn
            text: "☰"
            font.pixelSize: 18
            implicitWidth: 36
            implicitHeight: 36
            palette.buttonText: Theme.text
            background: Rectangle {
                radius: Theme.radiusSm
                color: menuBtn.hovered ? Theme.menuHover : "transparent"
            }
            onClicked: header.menuRequested(menuBtn)
        }
    }
}
