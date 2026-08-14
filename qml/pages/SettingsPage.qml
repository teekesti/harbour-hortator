import QtQuick 2.0
import Sailfish.Silica 1.0
import "components"

Page {
    id: settingsPage

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Settings")
            }

            Column {
                width: parent.width
                spacing: Theme.paddingSmall

                Label {
                    x: Theme.horizontalPageMargin
                    text: qsTr("Start delay")
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                }

                ValueAdjustmentHorizontal {
                    id: startDelayAdjustment
                    x: Theme.horizontalPageMargin
                    value: exerciseTimer.startDelay
                    minValue: 0
                    maxValue: 59
                    unitLabel: "s"
                }
                Binding { target: exerciseTimer; property: "startDelay"; value: startDelayAdjustment.value }
            }

            Column {
                width: parent.width
                spacing: Theme.paddingSmall

                Label {
                    x: Theme.horizontalPageMargin
                    text: qsTr("End-of-exercise warning")
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                }

                ValueAdjustmentHorizontal {
                    id: endWarningTimeAdjustment
                    x: Theme.horizontalPageMargin
                    value: exerciseTimer.endWarningTime
                    minValue: 0
                    maxValue: 59
                    unitLabel: "s"
                }
                Binding { target: exerciseTimer; property: "endWarningTime"; value: endWarningTimeAdjustment.value }
            }

            TextSwitch {
                text: qsTr("Mute all sounds")
                checked: exerciseTimer.muteSounds
                onCheckedChanged: exerciseTimer.muteSounds = checked
            }

            TextSwitch {
                text: qsTr("Skip last rest")
                checked: exerciseTimer.skipLastRest
                onCheckedChanged: exerciseTimer.skipLastRest = checked
            }
        }
    }
}
