import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ProjectO

RowLayout {
    id: actionBar
    spacing: 12

    readonly property int controlWidth: Theme.compactControlWidth

    property string desc: ""
    property bool showRotate: false
    property bool showConvertFormat: false
    property bool showCompress: false
    property bool showWatermark: false
    property int optionValue: 90
    property int watermarkCount: 1
    property string watermarkText: ""

    Text {
        Layout.fillWidth: true
        Layout.minimumWidth: 120
        Layout.maximumHeight: 40
        text: actionBar.desc
        elide: Text.ElideRight
        maximumLineCount: 1
        verticalAlignment: Text.AlignVCenter
        color: Theme.textBody
        font: Theme.mainFont
    }

    RowLayout {
        spacing: 10
        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

        WatermarkCountCombo {
            id: countPicker
            visible: actionBar.showWatermark
            Layout.preferredWidth: actionBar.controlWidth
            onCountValueChanged: actionBar.watermarkCount = countPicker.countValue
        }

        WatermarkInputField {
            id: watermarkInput
            visible: actionBar.showWatermark
            Layout.preferredWidth: 168
            Layout.preferredHeight: 38
            placeholderText: Theme.tr("watermarkPlaceholder")
            text: actionBar.watermarkText
            onTextEdited: function(value) { actionBar.watermarkText = value }
        }

        CompressComboBox {
            id: compressPicker
            visible: actionBar.showCompress
            Layout.preferredWidth: actionBar.controlWidth
            onLevelValueChanged: actionBar.optionValue = compressPicker.levelValue
        }

        ConvertComboBox {
            id: formatPicker
            visible: actionBar.showConvertFormat
            Layout.preferredWidth: actionBar.controlWidth
            onFormatValueChanged: actionBar.optionValue = formatPicker.formatValue
        }

        RotateComboBox {
            id: anglePicker
            visible: actionBar.showRotate
            Layout.preferredWidth: actionBar.controlWidth
            onRotateValueChanged: actionBar.optionValue = anglePicker.rotateValue
        }

        StyledButton {
            Layout.preferredWidth: actionBar.controlWidth
            text: Theme.tr("clear")
            enabled: AppController.fileCount > 0 && !AppController.busy
            onClicked: AppController.clearFiles()
        }

        StyledButton {
            Layout.preferredWidth: actionBar.controlWidth
            text: Theme.tr("run")
            highlighted: true
            enabled: AppController.fileCount > 0 && !AppController.busy
                     && (!actionBar.showWatermark || actionBar.watermarkText.trim().length > 0)
            onClicked: {
                if (actionBar.showWatermark)
                    AppController.runCurrentAction(actionBar.watermarkCount, actionBar.watermarkText)
                else
                    AppController.runCurrentAction(actionBar.optionValue, "")
            }
        }
    }
}
