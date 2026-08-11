import QtQuick 2.5
import Sailfish.Silica 1.0
import "./"

Rectangle {
    property bool isWorkout: exerciseTimer.currentActivity.activityType === "work"

    height: Theme.itemSizeExtraLarge
    radius: Theme.paddingSmall
    color: isWorkout ? UIConstants.workColor : UIConstants.restColor

    ProgressBar {
        anchors.fill: parent
        leftMargin: 0
        rightMargin: 0
        minimumValue: 0
        maximumValue: 1
        enabled: false
        value: exerciseTimer.currentProgress
        label: {
            var parts = []
            if (exerciseTimer.currentExerciseRoundCount > 1) {
                parts.push(qsTr("Round %1/%2").arg(exerciseTimer.currentExerciseRoundNumber).arg(exerciseTimer.currentExerciseRoundCount))
            }
            return parts.join(" · ")
        }
        valueText: Qt.formatTime(exerciseTimer.currentRunningTime, "mm:ss") + "/" +
                   Qt.formatTime(exerciseTimer.currentDuration, "mm:ss")
    }
}
