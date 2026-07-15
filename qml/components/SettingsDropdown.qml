import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import PageCase

Item {
    id: dropdown
    width: 280
    height: contentColumn.height + 16
    visible: opacity > 0.01
    opacity: 0
    scale: 0.94
    z: 2000

    transformOrigin: Item.TopRight

    Behavior on opacity {
        NumberAnimation { duration: Theme.animNormal; easing.type: Easing.OutCubic }
    }
    Behavior on scale {
        NumberAnimation { duration: Theme.animNormal; easing.type: Easing.OutCubic }
    }

    property alias contentHeight: contentColumn.height

    function openAt(anchor) {
        if (!anchor)
            return
        const pos = anchor.mapToItem(dropdown.parent, 0, anchor.height + 6)
        dropdown.x = Math.max(8, pos.x - dropdown.width + anchor.width)
        dropdown.y = pos.y
        dropdown.opacity = 1
        dropdown.scale = 1
        dropdown.forceActiveFocus()
    }

    function close() {
        dropdown.opacity = 0
        dropdown.scale = 0.94
    }

    ShadowCard {
        anchors.fill: parent
        radius: Theme.radiusMd
        margins: 0

        Column {
            id: contentColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 8
            spacing: 2

            Item {
                width: parent.width
                height: 28
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    text: Theme.tr("language")
                    font: Theme.captionBoldFont
                    color: Theme.textSecondary
                }
            }

            MenuOption {
                label: "简体中文"
                fontFamily: Theme.cjkFontFamily
                selected: AppSettings.language === "zh_CN"
                onTriggered: AppSettings.setLanguage("zh_CN")
            }
            MenuOption {
                label: "繁體中文"
                fontFamily: Theme.cjkFontFamily
                selected: AppSettings.language === "zh_TW"
                onTriggered: AppSettings.setLanguage("zh_TW")
            }
            MenuOption {
                label: "Français"
                fontFamily: Theme.latinFontFamily
                selected: AppSettings.language === "fr"
                onTriggered: AppSettings.setLanguage("fr")
            }
            MenuOption {
                label: "English"
                fontFamily: Theme.latinFontFamily
                selected: AppSettings.language === "en"
                onTriggered: AppSettings.setLanguage("en")
            }

            Rectangle {
                width: parent.width - 16
                anchors.horizontalCenter: parent.horizontalCenter
                height: 1
                color: Theme.border
            }

            Item {
                width: parent.width
                height: 28
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    text: Theme.tr("theme")
                    font: Theme.captionBoldFont
                    color: Theme.textSecondary
                }
            }

            Rectangle {
                width: parent.width - 16
                anchors.horizontalCenter: parent.horizontalCenter
                height: 36
                radius: Theme.radiusSm
                color: Theme.tabInactive

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 3
                    spacing: 3

                    ThemeToggleButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        label: Theme.tr("light")
                        selected: AppSettings.theme === "light"
                        onClicked: AppSettings.setTheme("light")
                    }
                    ThemeToggleButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        label: Theme.tr("dark")
                        selected: AppSettings.theme === "dark"
                        onClicked: AppSettings.setTheme("dark")
                    }
                }
            }

            Rectangle {
                width: parent.width - 16
                anchors.horizontalCenter: parent.horizontalCenter
                height: 1
                color: Theme.border
            }

            Item {
                width: parent.width
                height: 36

                Rectangle {
                    id: updateRow
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 8
                    height: 36
                    radius: Theme.radiusSm
                    color: updateHover ? Theme.menuHover : "transparent"

                    property bool updateHover: false

                    Text {
                        id: updateLabel
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        anchors.verticalCenter: parent.verticalCenter
                        text: Theme.tr("checkUpdate")
                        font.pixelSize: Theme.mainFont.pixelSize
                        font.family: Theme.mainFont.family
                        font.weight: Font.Medium
                        color: Theme.menuUnselectedText
                    }

                    UpdateStatusIcon {
                        anchors.left: updateLabel.right
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                        visible: UpdateChecker.status === 1
                                || UpdateChecker.status === 2
                                || UpdateChecker.status === 4
                                || UpdateChecker.status === 6
                    }

                    Text {
                        id: downloadProgressText
                        anchors.left: updateLabel.right
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                        visible: UpdateChecker.status === 5
                        text: UpdateChecker.downloadProgress + "%"
                        font.pixelSize: 11
                        font.family: Theme.mainFont.family
                        font.weight: Font.Normal
                        color: Theme.textSecondary
                    }

                    Rectangle {
                        id: installPill
                        z: 2
                        anchors.left: updateLabel.right
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                        height: 20
                        width: installPillText.implicitWidth + 12
                        radius: 10
                        color: Theme.accent
                        visible: UpdateChecker.status === 7

                        Text {
                            id: installPillText
                            anchors.centerIn: parent
                            text: Theme.tr("installUpdate")
                            color: "#FFFFFF"
                            font.pixelSize: 11
                            font.weight: Font.Normal
                        }

                        MouseArea {
                            z: 2
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: function(mouse) {
                                mouse.accepted = true
                                dropdown.close()
                                dropdown.installRequested()
                            }
                        }
                    }

                    MouseArea {
                        z: 1
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: installPill.visible ? installPill.x : parent.width
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: UpdateChecker.checkForUpdates()
                        onEntered: updateRow.updateHover = true
                        onExited: updateRow.updateHover = false
                    }
                }
            }

            MenuOption {
                label: Theme.tr("about")
                onTriggered: {
                    dropdown.close()
                    dropdown.aboutRequested()
                }
            }
        }
    }

    signal aboutRequested()
    signal installRequested()
}
