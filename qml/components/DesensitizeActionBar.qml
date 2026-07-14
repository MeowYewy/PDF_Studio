import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ProjectO

RowLayout {
    id: actionBar
    spacing: 12

    Text {
        Layout.fillWidth: true
        Layout.minimumWidth: 100
        Layout.maximumHeight: 44
        text: AppController.confirmed
              ? Theme.tr("workspaceDescReady")
              : Theme.tr("workspaceDesc")
        elide: Text.ElideRight
        maximumLineCount: 2
        wrapMode: Text.WordWrap
        verticalAlignment: Text.AlignVCenter
        color: Theme.textBody
        font: Theme.mainFont
    }

    RowLayout {
        spacing: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

        StyledButton {
            Layout.preferredWidth: 88
            text: Theme.tr("styleMosaic")
            highlighted: !AppController.useSolidBlock
            enabled: !AppController.busy
            onClicked: AppController.setUseSolidBlock(false)
        }

        StyledButton {
            Layout.preferredWidth: 88
            text: Theme.tr("styleBlock")
            highlighted: AppController.useSolidBlock
            enabled: !AppController.busy
            onClicked: AppController.setUseSolidBlock(true)
        }

        StyledButton {
            Layout.preferredWidth: 96
            text: AppController.manualMode ? Theme.tr("manualOn") : Theme.tr("manualOff")
            highlighted: AppController.manualMode
            enabled: AppController.confirmed && !AppController.busy
            onClicked: AppController.setManualMode(!AppController.manualMode)
        }

        StyledButton {
            Layout.preferredWidth: 72
            text: Theme.tr("clear")
            enabled: AppController.fileCount > 0 && !AppController.busy
            onClicked: AppController.clearFiles()
        }

        StyledButton {
            Layout.preferredWidth: 110
            text: Theme.tr("confirmAnalyze")
            highlighted: true
            enabled: AppController.fileCount > 0 && !AppController.busy
            onClicked: AppController.confirmAndAnalyze()
        }

        StyledButton {
            Layout.preferredWidth: 96
            text: Theme.tr("export")
            highlighted: true
            enabled: AppController.confirmed && !AppController.busy
            onClicked: AppController.exportDesensitized()
        }
    }
}
