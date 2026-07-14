import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ProjectO

Item {
    id: previewPanel
    clip: true

    property bool showRedactionOverlay: false
    property bool manualEditEnabled: false
    property int previewRotation: 0

    function fittedPreviewSize(slotW, slotH, aspect, rotation) {
        const ar = aspect > 0 ? aspect : 0.707
        let w = Math.min(slotW, slotH * ar)
        let h = w / ar
        const rad = rotation * Math.PI / 180.0
        const cos = Math.abs(Math.cos(rad))
        const sin = Math.abs(Math.sin(rad))
        const boundW = w * cos + h * sin
        const boundH = w * sin + h * cos
        const scale = Math.min(1, slotW / boundW, slotH / boundH)
        return { width: w * scale, height: h * scale }
    }

    readonly property real pageSlotHeight: stablePageSlot

    property real stablePageSlot: 320
    property int previewFileIndex: 0

    function syncPreviewIndex() {
        const paths = AppController.filePaths
        if (paths.length === 0) {
            previewFileIndex = 0
            return
        }
        const current = AppController.currentFileName
        for (let i = 0; i < paths.length; ++i) {
            const name = paths[i].replace(/\\/g, "/").split("/").pop()
            if (name === current) {
                previewFileIndex = i
                return
            }
        }
        previewFileIndex = 0
    }

    function showFileAt(index) {
        const paths = AppController.filePaths
        if (index < 0 || index >= paths.length)
            return
        previewFileIndex = index
        AppController.selectPreviewFile(paths[index])
    }

    function requestVisiblePages() {
        if (previewList.count === 0)
            return
        const rowHeight = previewPanel.pageSlotHeight + 28 + previewList.spacing
        if (rowHeight <= 0)
            return
        const first = Math.max(0, Math.floor(previewList.contentY / rowHeight))
        const last = Math.min(
            previewList.count - 1,
            Math.ceil((previewList.contentY + previewList.height) / rowHeight))
        AppController.ensurePreviewPagesLoaded(first + 1, last + 1 + 4)
    }

    function refreshStableSlot() {
        if (previewFrame.height > 0)
            stablePageSlot = Math.max(220, previewFrame.height - 32)
    }

    Timer {
        id: slotRefresh
        interval: 80
        repeat: false
        onTriggered: previewPanel.refreshStableSlot()
    }

    Connections {
        target: AppController
        function onFileCountChanged() { previewPanel.syncPreviewIndex() }
        function onPreviewChanged() {
            previewPanel.syncPreviewIndex()
            Qt.callLater(previewPanel.requestVisiblePages)
        }
    }

    Component.onCompleted: {
        syncPreviewIndex()
        refreshStableSlot()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            Layout.minimumHeight: 36
            spacing: 8

            Text {
                text: Theme.tr("preview")
                font: Theme.tabFont
                color: Theme.text
            }

            Text {
                visible: AppController.confirmed
                text: Theme.tr("regionsCount").replace("%1", AppController.regionCount)
                color: Theme.accent
                font: Theme.captionBoldFont
            }

            Item { Layout.fillWidth: true }

            StyledButton {
                visible: AppController.manualMode && AppController.regions.selectedIndex >= 0
                Layout.preferredWidth: 88
                text: Theme.tr("deleteMark")
                onClicked: AppController.regions.removeSelected()
            }

            RowLayout {
                spacing: 6
                opacity: AppController.fileCount > 0 ? 1 : 0
                enabled: AppController.fileCount > 0

                StyledButton {
                    Layout.preferredWidth: 32
                    text: "‹"
                    enabled: AppController.fileCount > 1
                    onClicked: {
                        const n = AppController.filePaths.length
                        showFileAt((previewFileIndex - 1 + n) % n)
                    }
                }

                Text {
                    Layout.preferredWidth: 180
                    Layout.minimumWidth: 180
                    Layout.maximumWidth: 180
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideMiddle
                    text: AppController.currentFileName.length > 0
                          ? AppController.currentFileName
                          : Theme.tr("noFiles")
                    color: Theme.text
                    font: Theme.mainFont
                }

                StyledButton {
                    Layout.preferredWidth: 32
                    text: "›"
                    enabled: AppController.fileCount > 1
                    onClicked: {
                        const n = AppController.filePaths.length
                        showFileAt((previewFileIndex + 1) % n)
                    }
                }
            }
        }

        Rectangle {
            id: previewFrame
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.radiusMd
            color: Theme.surfaceAlt
            border.color: Theme.border
            border.width: 1
            clip: true

            onHeightChanged: slotRefresh.restart()

            Item {
                anchors.fill: parent
                anchors.margins: 8

                ListView {
                    id: previewList
                    anchors.fill: parent
                    clip: true
                    spacing: 14
                    model: AppController.previewPages
                    opacity: AppController.previewPages.length > 0 ? 1 : 0
                    enabled: opacity > 0
                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AsNeeded
                    }

                    onContentYChanged: Qt.callLater(previewPanel.requestVisiblePages)
                    onHeightChanged: Qt.callLater(previewPanel.requestVisiblePages)
                    onCountChanged: Qt.callLater(previewPanel.requestVisiblePages)

                    delegate: Item {
                        required property var modelData
                        required property int index

                        width: previewList.width
                        height: previewPanel.pageSlotHeight + 28

                        Rectangle {
                            anchors.fill: parent
                            anchors.bottomMargin: 22
                            radius: Theme.radiusSm
                            color: Theme.surface
                            border.color: Theme.border
                            border.width: 1

                            Item {
                                id: slot
                                anchors.fill: parent
                                anchors.margins: 12
                                clip: true

                                readonly property real pageAspect:
                                    modelData.aspectRatio > 0 ? modelData.aspectRatio : 0.707
                                readonly property var fitSize: previewPanel.fittedPreviewSize(
                                    width, height, pageAspect, previewPanel.previewRotation)

                                Item {
                                    id: pageFrame
                                    anchors.centerIn: parent
                                    width: slot.fitSize.width
                                    height: slot.fitSize.height
                                    visible: !modelData.pending && (modelData.source || "").length > 0
                                    rotation: previewPanel.previewRotation
                                    transformOrigin: Item.Center
                                    clip: true

                                    Image {
                                        id: pageImage
                                        anchors.fill: parent
                                        source: modelData.source || ""
                                        fillMode: Image.PreserveAspectFit
                                        smooth: true
                                        cache: false
                                        asynchronous: true
                                    }

                                    RedactionOverlay {
                                        anchors.fill: parent
                                        visible: previewPanel.showRedactionOverlay
                                        pageNumber: modelData.pageNumber || (index + 1)
                                        manualEnabled: previewPanel.manualEditEnabled
                                    }
                                }

                                BusyIndicator {
                                    anchors.centerIn: parent
                                    running: modelData.pending === true
                                    visible: modelData.pending === true
                                }
                            }
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            text: modelData.label || ""
                            color: Theme.textBody
                            font: Theme.captionFont
                        }
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    radius: Theme.radiusSm
                    color: Theme.surface
                    border.color: Theme.border
                    border.width: 1
                    opacity: AppController.previewPages.length === 0
                             && !AppController.previewLoading ? 1 : 0
                    visible: opacity > 0

                    Text {
                        anchors.centerIn: parent
                        width: parent.width - 48
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                        text: Theme.tr("previewHint")
                        color: Theme.textBody
                        font: Theme.mainFont
                    }
                }

                Item {
                    anchors.fill: parent
                    visible: AppController.previewLoading
                    z: 2

                    Rectangle {
                        anchors.fill: parent
                        radius: Theme.radiusSm
                        color: Theme.surface
                        opacity: 0.85
                        border.color: Theme.border
                        border.width: 1
                    }

                    BusyIndicator {
                        anchors.centerIn: parent
                        running: parent.visible
                    }
                }
            }
        }
    }
}
