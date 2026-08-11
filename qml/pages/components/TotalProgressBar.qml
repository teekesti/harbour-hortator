import QtQuick 2.5
import Sailfish.Silica 1.0
import "./"

Item {
    property bool showLabel: true
    property color backgroundColor: "transparent"
    property real backgroundRadius: 0

    height: Theme.itemSizeExtraLarge

    Rectangle {
        anchors.fill: parent
        color: backgroundColor
        radius: backgroundRadius
    }

    SummaryBar {
        anchors.fill: parent
        playSequence: exerciseTimer.playSequenceSummary
        barHeight: parent.height
    }

    ProgressBar {
        anchors.fill: parent
        leftMargin: 0
        rightMargin: 0
        minimumValue: 0
        maximumValue: 1
        enabled: false
        value: exerciseTimer.totalProgress
        label: showLabel? UIConstants.totalProgressLabel : ""
        valueText: Qt.formatTime(exerciseTimer.totalRunningTime, "hh:mm:ss") + "/" +
                   Qt.formatTime(exerciseTimer.totalDuration, "hh:mm:ss")
    }
}
