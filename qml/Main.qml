import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ProjectO

ApplicationWindow {
    id: root
    width: 1180
    height: 760
    minimumWidth: 960
    minimumHeight: 640
    visible: true
    title: Theme.tr("appName")
    color: Theme.bg

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

            WorkspacePage {
                anchors.fill: parent
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
    }

    SettingsDropdown {
        id: settingsDropdown
        parent: root.contentItem
        onAboutRequested: aboutDialog.open()
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
}
