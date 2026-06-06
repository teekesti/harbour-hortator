import QtQuick 2.0
import Sailfish.Silica 1.0

Row {
    property int value: 0
    property int minValue: 0
    property int maxValue: 100
    property int step: 1
    property string unitLabel: ""
    spacing: Theme.paddingSmall

    AcceleratingIconButton {
        id: decrementButton
        anchors.verticalCenter: parent.verticalCenter
        iconSource: "image://theme/icon-m-remove"
        enabled: value > minValue
        onTriggered: value = Math.max(value - step, minValue)
    }

    Label {
       text: value.toString() + " " + unitLabel
       font.pixelSize: Theme.fontSizeMedium
       anchors.verticalCenter: parent.verticalCenter
    }

    AcceleratingIconButton {
        id: incrementButton
        anchors.verticalCenter: parent.verticalCenter
        iconSource: "image://theme/icon-m-add"
        enabled:  value < maxValue
        onTriggered: value = Math.min(value + step, maxValue)
    }


}
