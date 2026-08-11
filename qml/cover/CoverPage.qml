import QtQuick 2.0
import Sailfish.Silica 1.0
import "../pages/components"

CoverBackground {
    Image {
        source: Qt.resolvedUrl("../images/kettlebell_stopwatch_with_background.png")
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
    }
    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: Theme.paddingMedium
        }
        spacing: Theme.paddingMedium

        Label {
            width: parent.width
            text: qsTr(applicationDisplayName)
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.primaryColor
            truncationMode: TruncationMode.Fade
        }

        Column {
            width: parent.width
            spacing: Theme.paddingSmall / 2

            Label {
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: UIConstants.currentActivityLabel
            }

            Rectangle {
                width: parent.width
                height: Theme.paddingSmall
                color: Theme.rgba(Theme.primaryColor, 0.2)
                radius: height / 2

                Rectangle {
                    width: parent.width * exerciseTimer.currentProgress
                    height: parent.height
                    color: Theme.highlightColor
                    radius: parent.radius
                }
            }
        }

        Column {
            width: parent.width
            spacing: Theme.paddingSmall / 2

            Label {
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: UIConstants.totalProgressLabel
            }

            Rectangle {
                width: parent.width
                height: Theme.paddingSmall
                color: Theme.rgba(Theme.primaryColor, 0.2)
                radius: height / 2

                Rectangle {
                    width: parent.width * exerciseTimer.totalProgress
                    height: parent.height
                    color: Theme.primaryColor
                    radius: parent.radius
                }
            }
        }
    }

    CoverActionList {
        enabled: !exerciseTimer.running && exerciseTimer.allValid

        CoverAction {
            iconSource: "image://theme/icon-cover-play"
            onTriggered: exerciseTimer.start()
        }
    }

    CoverActionList {
        enabled: exerciseTimer.running

        CoverAction {
            iconSource: "image://theme/icon-cover-pause"
            onTriggered: exerciseTimer.pause()
        }
    }
}
