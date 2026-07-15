import QtQuick
import QtQuick.Controls
import PageCase

Item {
    id: dropHost
    implicitHeight: 108

    property bool dragActive: false
    property string hintText: Theme.tr("dropHint")
    property int incomingCount: 0

    readonly property real dragScale: dragActive ? 1.025 : 1.0

    function toLocalPath(url) {
        if (!url)
            return ""
        if (typeof url === "string") {
            let s = url
            if (s.startsWith("file:///"))
                s = s.slice(8)
            else if (s.startsWith("file://"))
                s = s.slice(7)
            try {
                return decodeURIComponent(s)
            } catch (e) {
                return s
            }
        }
        if (typeof url.toLocalFile === "function")
            return url.toLocalFile()
        return url.toString ? url.toString() : ""
    }

    Rectangle {
        id: shadowFar
        anchors.fill: card
        anchors.topMargin: dropHost.dragActive ? 10 : 3
        radius: Theme.radiusMd
        color: Theme.shadowColor
        opacity: dropHost.dragActive ? Theme.shadowOpacity2 : 0
        z: 0

        Behavior on opacity { NumberAnimation { duration: Theme.animNormal; easing.type: Easing.OutCubic } }
        Behavior on anchors.topMargin { NumberAnimation { duration: Theme.animNormal; easing.type: Easing.OutCubic } }
    }

    Rectangle {
        id: shadowNear
        anchors.fill: card
        anchors.topMargin: dropHost.dragActive ? 5 : 2
        radius: Theme.radiusMd
        color: Theme.shadowColor
        opacity: dropHost.dragActive ? Theme.shadowOpacity1 : 0
        z: 1

        Behavior on opacity { NumberAnimation { duration: Theme.animNormal; easing.type: Easing.OutCubic } }
        Behavior on anchors.topMargin { NumberAnimation { duration: Theme.animNormal; easing.type: Easing.OutCubic } }
    }

    Rectangle {
        id: card
        anchors.fill: parent
        z: 2
        radius: Theme.radiusMd
        color: dropHost.dragActive ? Theme.surfaceAlt : Theme.surface
        border.color: Theme.border
        border.width: 1
        scale: dropHost.dragScale
        transformOrigin: Item.Center

        Behavior on color { ColorAnimation { duration: Theme.animNormal } }
        Behavior on scale {
            NumberAnimation { duration: Theme.animNormal; easing.type: Easing.OutCubic }
        }

        DropArea {
            anchors.fill: parent
            keys: ["text/uri-list"]
            onEntered: function(drag) {
                dropHost.dragActive = true
                dropHost.incomingCount = drag.urls ? drag.urls.length : 0
                drag.accept(Qt.CopyAction)
            }
            onExited: {
                dropHost.dragActive = false
                dropHost.incomingCount = 0
            }
            onDropped: function(drop) {
                dropHost.dragActive = false
                drop.accept(Qt.CopyAction)
                if (!drop.hasUrls)
                    return

                const paths = []
                for (let i = 0; i < drop.urls.length; ++i) {
                    const local = dropHost.toLocalPath(drop.urls[i])
                    if (local.length > 0)
                        paths.push(local)
                }
                dropHost.incomingCount = paths.length
                if (paths.length > 0)
                    AppController.addFilesFromList(paths)

                dropPulse.restart()
            }
        }

        Column {
            anchors.centerIn: parent
            width: parent.width - 24
            spacing: 10

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: dropHost.dragActive && dropHost.incomingCount > 0
                      ? ("+" + dropHost.incomingCount + " " + Theme.tr("filesAdded"))
                      : dropHost.hintText
                color: dropHost.dragActive ? Theme.text : Theme.textBody
                font: dropHost.dragActive ? Theme.mainFontBold : Theme.mainFont
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                width: parent.width

                Behavior on color { ColorAnimation { duration: Theme.animFast } }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                visible: !dropHost.dragActive && AppController.fileCount > 0
                text: AppController.fileCount + " " + Theme.tr("filesAdded")
                color: Theme.accent
                font: Theme.captionFont
            }

            StyledButton {
                anchors.horizontalCenter: parent.horizontalCenter
                text: Theme.tr("browse")
                highlighted: true
                onClicked: AppController.browseAndAddFiles()
            }
        }
    }

    SequentialAnimation {
        id: dropPulse
        NumberAnimation {
            target: card
            property: "scale"
            to: 1.03
            duration: Theme.animFast
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            target: card
            property: "scale"
            to: dropHost.dragScale
            duration: Theme.animNormal
            easing.type: Easing.OutCubic
        }
    }
}
