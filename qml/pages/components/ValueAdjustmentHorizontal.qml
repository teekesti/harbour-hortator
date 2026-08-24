import QtQuick 2.4
import Sailfish.Silica 1.0

Rectangle {
    id: root
    property int value: 0
    property int minValue: 0
    property int maxValue: 100
    property int step: 1
    property string unitLabel: ""

    width: contentRow.width + 2 * Theme.paddingSmall
    height: contentRow.height
    radius: Theme.paddingSmall
    color: Theme.rgba(Theme.highlightBackgroundColor, 0.15)

    TextMetrics {
        id: maxTextMetrics
        font.pixelSize: Theme.fontSizeMedium
        text: root.maxValue.toString() + " " + root.unitLabel
    }

    Row {
        id: contentRow
        anchors.centerIn: parent
        spacing: 0

        AcceleratingIconButton {
            id: decrementButton
            anchors.verticalCenter: parent.verticalCenter
            iconSource: "image://theme/icon-m-remove"
            enabled: root.value > root.minValue
            onTriggered: root.value = Math.max(root.value - root.step, root.minValue)
        }

        Label {
           text: root.value.toString() + " " + root.unitLabel
           font.pixelSize: Theme.fontSizeMedium
           width: maxTextMetrics.width
           horizontalAlignment: Text.AlignHCenter
           anchors.verticalCenter: parent.verticalCenter
        }

        AcceleratingIconButton {
            id: incrementButton
            anchors.verticalCenter: parent.verticalCenter
            iconSource: "image://theme/icon-m-add"
            enabled: root.value < root.maxValue
            onTriggered: root.value = Math.min(root.value + root.step, root.maxValue)
        }
    }
}
