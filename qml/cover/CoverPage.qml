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
        spacing: Theme.paddingLarge

        Label {
            width: parent.width
            text: qsTr(applicationDisplayName)
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.primaryColor
            truncationMode: TruncationMode.Fade
        }

        TotalProgressBar {
            width: parent.width
            showLabel: false
            backgroundColor: Theme.rgba(Theme.overlayBackgroundColor, 0.7)
            backgroundRadius: Theme.paddingSmall
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
