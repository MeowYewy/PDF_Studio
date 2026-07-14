import QtQuick
import QtQuick.Controls
import ProjectO

Rectangle {
    id: toast
    visible: opacity > 0
    opacity: 0
    radius: Theme.radiusMd
    color: Theme.surfaceAlt
    border.color: ok ? Theme.success : Theme.danger
    border.width: 1
    width: Math.min(360, Math.max(160, label.implicitWidth + 32))
    height: 44

    property bool ok: true

    function show(message, success) {
        ok = success
        label.text = message
        fadeIn.restart()
        hideTimer.restart()
    }

    Text {
        id: label
        anchors.centerIn: parent
        font: Theme.mainFont
        color: toast.ok ? Theme.success : Theme.danger
    }

    SequentialAnimation {
        id: fadeIn
        NumberAnimation { target: toast; property: "opacity"; to: 1; duration: Theme.animNormal }
    }

    Timer {
        id: hideTimer
        interval: 2400
        onTriggered: fadeOut.start()
    }

    NumberAnimation {
        id: fadeOut
        target: toast
        property: "opacity"
        to: 0
        duration: Theme.animNormal
    }
}
