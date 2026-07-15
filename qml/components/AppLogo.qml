import QtQuick

Item {
    id: root

    property int logoSize: 32
    property real cornerRadius: 8

    implicitWidth: logoSize
    implicitHeight: logoSize
    width: logoSize
    height: logoSize

    Image {
        id: logoImage
        anchors.fill: parent
        source: "qrc:/qt/qml/PageCase/resources/logo.svg"
        sourceSize: Qt.size(root.logoSize * 2, root.logoSize * 2)
        fillMode: Image.PreserveAspectFit
        smooth: true
        mipmap: true

        onStatusChanged: {
            if (status === Image.Error)
                logoImage.source = "qrc:/qt/qml/PageCase/resources/app-icon.png"
        }
    }
}
