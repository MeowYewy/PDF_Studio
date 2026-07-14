import QtQuick
import QtQuick.Controls
import ProjectO

Item {
    id: overlay
    anchors.fill: parent

    property int pageNumber: 1
    property bool manualEnabled: false
    property bool showMosaicPreview: true

    readonly property string filePath: AppController.currentFilePath
    readonly property var pageRegions: {
        const _ = AppController.regionCount
        const __ = AppController.confirmed
        if (!filePath || pageNumber < 1)
            return []
        return AppController.regionsForCurrentPage(pageNumber)
    }

    property real dragStartX: 0
    property real dragStartY: 0
    property real dragCurX: 0
    property real dragCurY: 0
    property bool drawing: false
    property int activeHandle: -1 // 0 TL 1 TR 2 BR 3 BL 4 move
    property int editingIndex: -1
    property real editOrigNx: 0
    property real editOrigNy: 0
    property real editOrigNw: 0
    property real editOrigNh: 0
    property real editStartX: 0
    property real editStartY: 0

    function normFromPixels(px, py, pw, ph) {
        return {
            x: Math.max(0, Math.min(1, px / Math.max(1, width))),
            y: Math.max(0, Math.min(1, py / Math.max(1, height))),
            w: Math.max(0.004, Math.min(1, pw / Math.max(1, width))),
            h: Math.max(0.004, Math.min(1, ph / Math.max(1, height)))
        }
    }

    function kindColor(kind) {
        if (kind === "idCard") return "#E11D48"
        if (kind === "name") return "#D97706"
        if (kind === "phone") return "#2563EB"
        if (kind === "address") return "#059669"
        return Theme.accent
    }

    // Draw new region (behind region editors so existing marks stay interactive)
    MouseArea {
        z: 0
        anchors.fill: parent
        enabled: overlay.manualEnabled
        acceptedButtons: Qt.LeftButton
        onPressed: function(mouse) {
            overlay.drawing = true
            overlay.dragStartX = mouse.x
            overlay.dragStartY = mouse.y
            overlay.dragCurX = mouse.x
            overlay.dragCurY = mouse.y
        }
        onPositionChanged: function(mouse) {
            if (!overlay.drawing)
                return
            overlay.dragCurX = mouse.x
            overlay.dragCurY = mouse.y
        }
        onReleased: function(mouse) {
            if (!overlay.drawing)
                return
            overlay.drawing = false
            const x = Math.min(overlay.dragStartX, overlay.dragCurX)
            const y = Math.min(overlay.dragStartY, overlay.dragCurY)
            const w = Math.abs(overlay.dragCurX - overlay.dragStartX)
            const h = Math.abs(overlay.dragCurY - overlay.dragStartY)
            if (w < 8 || h < 8)
                return
            const n = overlay.normFromPixels(x, y, w, h)
            AppController.regions.addManualRegion(
                overlay.filePath, overlay.pageNumber, n.x, n.y, n.w, n.h)
        }
    }

    Rectangle {
        z: 1
        visible: overlay.drawing
        color: "#8A5CF555"
        border.color: Theme.accent
        border.width: 1
        x: Math.min(overlay.dragStartX, overlay.dragCurX)
        y: Math.min(overlay.dragStartY, overlay.dragCurY)
        width: Math.abs(overlay.dragCurX - overlay.dragStartX)
        height: Math.abs(overlay.dragCurY - overlay.dragStartY)
    }

    // Burned mosaic / solid preview on top of page image
    Repeater {
        model: overlay.pageRegions

        Item {
            id: regionItem
            z: 2
            required property var modelData

            visible: modelData.enabled !== false
            x: modelData.nx * overlay.width
            y: modelData.ny * overlay.height
            width: Math.max(2, modelData.nw * overlay.width)
            height: Math.max(2, modelData.nh * overlay.height)

            // Preview of irreversible redaction (export applies real pixelation)
            Rectangle {
                anchors.fill: parent
                color: AppController.useSolidBlock ? "#141418" : "#2C2C32"
                opacity: 0.94
                radius: 2
            }
            Canvas {
                anchors.fill: parent
                visible: !AppController.useSolidBlock
                onPaint: {
                    const ctx = getContext("2d")
                    const s = 8
                    for (let y = 0; y < height; y += s) {
                        for (let x = 0; x < width; x += s) {
                            const odd = ((x / s + y / s) % 2 === 0)
                            ctx.fillStyle = odd ? "#2A2A30" : "#3A3A42"
                            ctx.fillRect(x, y, s, s)
                        }
                    }
                }
                onWidthChanged: requestPaint()
                onHeightChanged: requestPaint()
            }

            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.width: modelData.selected ? 2 : 1
                border.color: modelData.selected ? "#FFFFFF"
                                                 : kindColor(modelData.kind)
                radius: 2
            }

            Text {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 2
                text: modelData.label || modelData.kind
                color: "#FFFFFF"
                font.pixelSize: 10
                font.family: Theme.uiFontFamily
                style: Text.Outline
                styleColor: "#00000088"
            }

            MouseArea {
                anchors.fill: parent
                enabled: overlay.manualEnabled
                hoverEnabled: true
                cursorShape: Qt.SizeAllCursor
                onPressed: function(mouse) {
                    AppController.regions.selectAt(modelData.index)
                    overlay.editingIndex = modelData.index
                    overlay.activeHandle = 4
                    overlay.editStartX = mouse.x
                    overlay.editStartY = mouse.y
                    overlay.editOrigNx = modelData.nx
                    overlay.editOrigNy = modelData.ny
                    overlay.editOrigNw = modelData.nw
                    overlay.editOrigNh = modelData.nh
                }
                onPositionChanged: function(mouse) {
                    if (overlay.activeHandle !== 4 || overlay.editingIndex < 0)
                        return
                    const dx = (mouse.x - overlay.editStartX) / Math.max(1, overlay.width)
                    const dy = (mouse.y - overlay.editStartY) / Math.max(1, overlay.height)
                    AppController.regions.setRegionRect(
                        overlay.editingIndex,
                        Math.max(0, Math.min(1 - overlay.editOrigNw, overlay.editOrigNx + dx)),
                        Math.max(0, Math.min(1 - overlay.editOrigNh, overlay.editOrigNy + dy)),
                        overlay.editOrigNw,
                        overlay.editOrigNh)
                }
                onReleased: {
                    overlay.activeHandle = -1
                    overlay.editingIndex = -1
                }
                onClicked: AppController.regions.selectAt(modelData.index)
            }

            Repeater {
                model: 4
                Rectangle {
                    required property int index
                    width: 8
                    height: 8
                    radius: 2
                    color: "#FFFFFF"
                    border.color: Theme.accent
                    visible: overlay.manualEnabled && modelData.selected
                    x: index === 0 || index === 3 ? -4 : parent.width - 4
                    y: index === 0 || index === 1 ? -4 : parent.height - 4
                    z: 5

                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: -4
                        cursorShape: Qt.SizeFDiagCursor
                        onPressed: function(mouse) {
                            overlay.editingIndex = modelData.index
                            overlay.activeHandle = index
                            overlay.editStartX = mapToItem(overlay, mouse.x, mouse.y).x
                            overlay.editStartY = mapToItem(overlay, mouse.x, mouse.y).y
                            overlay.editOrigNx = modelData.nx
                            overlay.editOrigNy = modelData.ny
                            overlay.editOrigNw = modelData.nw
                            overlay.editOrigNh = modelData.nh
                        }
                        onPositionChanged: function(mouse) {
                            if (overlay.editingIndex < 0)
                                return
                            const p = mapToItem(overlay, mouse.x, mouse.y)
                            const nx = p.x / Math.max(1, overlay.width)
                            const ny = p.y / Math.max(1, overlay.height)
                            let x = overlay.editOrigNx
                            let y = overlay.editOrigNy
                            let w = overlay.editOrigNw
                            let h = overlay.editOrigNh
                            const right = overlay.editOrigNx + overlay.editOrigNw
                            const bottom = overlay.editOrigNy + overlay.editOrigNh
                            if (index === 0) {
                                x = Math.min(nx, right - 0.01)
                                y = Math.min(ny, bottom - 0.01)
                                w = right - x
                                h = bottom - y
                            } else if (index === 1) {
                                y = Math.min(ny, bottom - 0.01)
                                w = Math.max(0.01, nx - overlay.editOrigNx)
                                h = bottom - y
                            } else if (index === 2) {
                                w = Math.max(0.01, nx - overlay.editOrigNx)
                                h = Math.max(0.01, ny - overlay.editOrigNy)
                            } else {
                                x = Math.min(nx, right - 0.01)
                                w = right - x
                                h = Math.max(0.01, ny - overlay.editOrigNy)
                            }
                            AppController.regions.setRegionRect(overlay.editingIndex, x, y, w, h)
                        }
                        onReleased: {
                            overlay.activeHandle = -1
                            overlay.editingIndex = -1
                        }
                    }
                }
            }
        }
    }

    Keys.onPressed: function(event) {
        if (!overlay.manualEnabled)
            return
        if (event.key === Qt.Key_Delete || event.key === Qt.Key_Backspace) {
            AppController.regions.removeSelected()
            event.accepted = true
        }
    }
    focus: manualEnabled
}
