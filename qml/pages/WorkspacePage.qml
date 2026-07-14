import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ProjectO

Item {
    id: pageBase
    clip: true

    ShadowCard {
        anchors.fill: parent
        radius: Theme.radiusLg
        margins: 0

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 0

            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 24

                ColumnLayout {
                    Layout.preferredWidth: 300
                    Layout.maximumWidth: 320
                    Layout.fillHeight: true
                    spacing: 12

                    Text {
                        text: Theme.tr("fileList")
                        font: Theme.tabFont
                        color: Theme.text
                    }

                    FileDropZone {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Text {
                            text: Theme.tr("sortMode")
                            font: Theme.captionFont
                            color: Theme.textBody
                        }

                        Item { Layout.fillWidth: true }

                        StyledButton {
                            Layout.preferredWidth: 72
                            text: Theme.tr("sortMixed")
                            highlighted: AppController.files.sortMode === "mixed"
                            onClicked: AppController.setSortMode("mixed")
                        }

                        StyledButton {
                            Layout.preferredWidth: 72
                            text: Theme.tr("sortCategory")
                            highlighted: AppController.files.sortMode === "category"
                            onClicked: AppController.setSortMode("category")
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        radius: Theme.radiusMd
                        color: Theme.surfaceAlt
                        border.color: Theme.border
                        border.width: 1
                        clip: true

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 8

                            RowLayout {
                                Layout.fillWidth: true

                                Text {
                                    text: Theme.tr("addedFiles")
                                    font: Theme.captionFont
                                    color: Theme.textBody
                                }

                                Item { Layout.fillWidth: true }

                                Text {
                                    visible: AppController.fileCount > 0
                                    text: AppController.fileCount.toString()
                                    color: Theme.accent
                                    font: Theme.captionBoldFont
                                }
                            }

                            FileListView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                enableReorder: true
                                showCategory: true
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 1
                    Layout.fillHeight: true
                    color: Theme.border
                }

                ScrollPreview {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    showRedactionOverlay: AppController.confirmed
                    manualEditEnabled: AppController.manualMode
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                Layout.topMargin: 20
                Layout.bottomMargin: 16
                color: Theme.border
            }

            DesensitizeActionBar {
                Layout.fillWidth: true
                Layout.preferredHeight: 44
            }
        }
    }
}
