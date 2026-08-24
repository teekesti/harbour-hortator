import QtQuick 2.0
import Sailfish.Silica 1.0

Rectangle {
    id: root
    property int value: 1
    property int minValue: 1
    property int maxValue: 100
    property int step: 1

    width: contentRow.width + 2 * Theme.paddingSmall
    height: contentRow.height
    radius: Theme.paddingSmall
    color: Theme.rgba(Theme.highlightBackgroundColor, 0.15)

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

        Item {
            id: roundIndicator
            property real iconScaleFactor: 1.3
            property real labelScaleFactor: 1.0
            anchors.verticalCenter: parent.verticalCenter
            width: Theme.iconSizeMedium * iconScaleFactor
            height: Theme.iconSizeMedium * iconScaleFactor

            Icon {
                anchors.fill: parent
                source: "image://theme/icon-m-repeat"
            }

            Label {
                anchors.centerIn: parent
                text: root.value.toString()
                font.pixelSize: Theme.fontSizeExtraSmall * roundIndicator.labelScaleFactor
                color: Theme.primaryColor
            }
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
