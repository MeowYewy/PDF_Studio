import QtQuick
import QtQuick.Effects
import PageCase

Item {
    id: root

    property string icon: ""
    property color color: Theme.textBody
    property real iconOpacity: 1

    implicitWidth: 18
    implicitHeight: 18

    readonly property url iconSource: icon.length > 0
        ? ("qrc:/qt/qml/PageCase/resources/icons/" + icon + ".svg")
        : ""

    // White-stroke SVGs + colorization (no brightness — that washes purple toward pink).
    Image {
        id: img
        anchors.fill: parent
        source: root.iconSource
        fillMode: Image.PreserveAspectFit
        sourceSize.width: Math.max(48, Math.round(width * 3))
        sourceSize.height: Math.max(48, Math.round(height * 3))
        asynchronous: true
        smooth: true
        cache: true
        opacity: root.iconOpacity
        layer.enabled: true
        layer.smooth: true
        layer.effect: MultiEffect {
            colorization: 1.0
            colorizationColor: root.color
        }
    }
}
