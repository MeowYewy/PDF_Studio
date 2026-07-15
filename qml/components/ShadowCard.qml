import QtQuick
import PageCase

Item {
    id: root

    property alias radius: card.radius
    property alias color: card.color
    property alias border: card.border
    property int margins: 0
    default property alias contents: inner.data

    implicitWidth: inner.implicitWidth + margins * 2
    implicitHeight: inner.implicitHeight + margins * 2

    Rectangle {
        anchors.fill: card
        anchors.topMargin: Theme.shadowOffset1
        radius: card.radius
        color: Theme.shadowColor
        opacity: Theme.shadowOpacity1
        z: -2
    }

    Rectangle {
        anchors.fill: card
        anchors.topMargin: Theme.shadowOffset2
        radius: card.radius
        color: Theme.shadowColor
        opacity: Theme.shadowOpacity2
        z: -3
    }

    Rectangle {
        id: card
        anchors.fill: parent
        radius: Theme.radiusLg
        color: Theme.surface
        border.color: Theme.border
        border.width: 1

        Item {
            id: inner
            anchors.fill: parent
            anchors.margins: root.margins
        }
    }
}
