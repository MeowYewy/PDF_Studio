import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import PageCase

ApplicationWindow {
    id: root
    width: 1180
    height: 760
    minimumWidth: 960
    minimumHeight: 640
    visible: true
    title: Theme.tr("appName")
    color: Theme.bg

    property int activeTab: 0

    palette: Palette {
        window: Theme.bg
        windowText: Theme.text
        base: Theme.surface
        text: Theme.text
        button: Theme.surfaceAlt
        buttonText: Theme.text
        highlight: Theme.accent
        highlightedText: "#FFFFFF"
        alternateBase: Theme.surfaceAlt
    }

    // Sits behind all content: clicks on empty areas that no control handles
    // land here and clear the active focus (e.g. the page-range field), so
    // its accent border fades out just like the file picker search box.
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: function(mouse) {
            root.contentItem.forceActiveFocus()
            mouse.accepted = false
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        AppHeader {
            Layout.fillWidth: true
            Layout.preferredHeight: Theme.headerHeight
            onMenuRequested: function(anchor) {
                if (settingsDropdown.opacity > 0.5)
                    settingsDropdown.close()
                else
                    settingsDropdown.openAt(anchor)
            }
        }

        GlobalProgressBar {
            Layout.fillWidth: true
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 32

            ColumnLayout {
                anchors.fill: parent
                spacing: 20

                FeatureTabs {
                    Layout.alignment: Qt.AlignHCenter
                    currentIndex: root.activeTab
                    onTabChanged: function(index) {
                        root.activeTab = index
                        AppController.currentTab = index
                    }
                }

                StackLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: root.activeTab

                    SplitPage { Layout.fillWidth: true; Layout.fillHeight: true }
                    MergePage { Layout.fillWidth: true; Layout.fillHeight: true }
                    RotatePage { Layout.fillWidth: true; Layout.fillHeight: true }
                    ConvertPage { Layout.fillWidth: true; Layout.fillHeight: true }
                    CompressPage { Layout.fillWidth: true; Layout.fillHeight: true }
                    WatermarkPage { Layout.fillWidth: true; Layout.fillHeight: true }
                }
            }
        }
    }

    StatusToast {
        id: statusToast
        parent: root.contentItem
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: Theme.headerHeight
                         + (AppController.processing || AppController.progress > 0 ? 3 : 0)
                         + 12
        anchors.rightMargin: 32
        z: 2500
    }

    Connections {
        target: AppController
        function onActionFinished(ok, message) {
            if (ok)
                statusToast.show(message, true)
            else
                statusToast.show(message, false)
        }
    }

    MouseArea {
        anchors.fill: parent
        visible: settingsDropdown.opacity > 0.5
        z: 1999
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: settingsDropdown.close()
        onWheel: function(wheel) { wheel.accepted = true }
    }

    SettingsDropdown {
        id: settingsDropdown
        parent: root.contentItem
        onAboutRequested: aboutDialog.open()
        onInstallRequested: installConfirmPopup.open()
    }

    Popup {
        id: installConfirmPopup
        parent: Overlay.overlay
        modal: true
        dim: true
        padding: 20
        width: 360
        anchors.centerIn: parent
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        transformOrigin: Item.Center
        opacity: 1
        scale: 1

        enter: Transition {
            ParallelAnimation {
                NumberAnimation { property: "opacity"; from: 0; to: 1; duration: Theme.animNormal; easing.type: Easing.OutCubic }
                NumberAnimation { property: "scale"; from: 0.94; to: 1; duration: Theme.animSlow; easing.type: Easing.OutBack; easing.overshoot: 1.08 }
            }
        }
        exit: Transition {
            ParallelAnimation {
                NumberAnimation { property: "opacity"; to: 0; duration: Theme.animFast; easing.type: Easing.InCubic }
                NumberAnimation { property: "scale"; to: 0.96; duration: Theme.animFast; easing.type: Easing.InCubic }
            }
        }

        background: Rectangle {
            radius: Theme.radiusMd
            color: Theme.surface
            border.color: Theme.border
            border.width: 1
        }

        contentItem: ColumnLayout {
            spacing: 14

            Text {
                Layout.fillWidth: true
                text: Theme.tr("installConfirmTitle")
                font: Theme.mainFontBold
                color: Theme.text
            }

            Text {
                Layout.fillWidth: true
                wrapMode: Text.Wrap
                text: Theme.tr("installConfirmMessage")
                font: Theme.mainFont
                color: Theme.textBody
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                Item { Layout.fillWidth: true }
                StyledButton {
                    text: Theme.tr("pickerCancel")
                    onClicked: installConfirmPopup.close()
                }
                StyledButton {
                    text: Theme.tr("installUpdate")
                    highlighted: true
                    onClicked: {
                        installConfirmPopup.close()
                        UpdateChecker.installUpdate()
                    }
                }
            }
        }
    }

    AboutDialog {
        id: aboutDialog
        parent: Overlay.overlay
        anchors.fill: parent
        onChangelogRequested: {
            aboutDialog.close()
            changelogDialog.open()
        }
    }

    ChangelogDialog {
        id: changelogDialog
        parent: Overlay.overlay
        anchors.fill: parent
    }

    FilePickerDialog {
        parent: Overlay.overlay
        anchors.fill: parent
    }

    Component.onCompleted: {
        // Quiet background check; auto-downloads when a newer version exists.
        Qt.callLater(function() { UpdateChecker.checkForUpdates() })
    }
}
