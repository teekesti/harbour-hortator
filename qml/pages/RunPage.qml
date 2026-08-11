import QtQuick 2.5
import Sailfish.Silica 1.0
import QtQuick.Layouts 1.1 // Required for RowLayout
import Nemo.KeepAlive 1.2 // To prevent screen blanking
import "components"

Page {
    id: runPage
    objectName: "runPage"

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All
    property bool isWorkout: exerciseTimer.currentActivity.activityType === "work"

    DisplayBlanking {
            preventBlanking: runPage.status === PageStatus.Active && exerciseTimer.running
        }

    KeepAlive {
        // Keeps CPU awake so background audio/timers work if screen turns off
        enabled: exerciseTimer.running
    }

    Flow {
        flow: orientation == Orientation.Portrait ? Flow.TopToBottom : Flow.LeftToRight
        anchors.fill: parent

        Rectangle {
            id: countDownRect
            //anchors.horizontalCenter: parent.horizontalCenter
            width: orientation == Orientation.Portrait ? runPage.width : runPage.width / 2
            height: width
            color: Theme.backgroundGlowColor

            Label { // Display a large countdown number
                    id: countdownNumber
                    //anchors.centerIn: parent
                    anchors.fill: parent
                    color: Theme.highlightColor
                    font.bold: true
                    style: Text.Raised
                    font.pixelSize: parent.height * 0.8
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    // Only shown during countdown
                    opacity: 0
                    SequentialAnimation  {
                    id: flashCountdownNumber
                    NumberAnimation {id: showAnimation; target: countdownNumber;
                        property: "opacity"; easing.type: Easing.InOutExpo; to: 60; duration: 200 }
                    NumberAnimation {id: hideAnimation; target: countdownNumber;
                        property: "opacity"; easing.type: Easing.InOutExpo; to: 0; duration: 500 }
                    }

                }

                Connections {
                    target: exerciseTimer
                    onCountDown: {
                        if (number > 0)
                        {
                            countdownNumber.text = number // number from signal parameter
                            flashCountdownNumber.start()
                        }
                        else
                        {
                            countdownNumber.opacity = 0
                        }
                    }
                }

        }

        Item {
            width: countDownRect.width
            height: countDownRect.height

            Column {
                spacing: Theme.paddingMedium
                anchors.horizontalCenter: parent.horizontalCenter

                Label {
                    id: positionLabel
                    width: countDownRect.width - 2 * Theme.horizontalPageMargin
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.secondaryColor
                    visible: text.length > 0
                    text: {
                        var parts = [UIConstants.currentActivityLabel]
                        if (exerciseTimer.currentSetCount > 1) {
                            parts.push(qsTr("Set %1/%2").arg(exerciseTimer.currentSetNumber).arg(exerciseTimer.currentSetCount))
                        }
                        if (exerciseTimer.currentSetRoundCount > 1) {
                            parts.push(qsTr("Set round %1/%2").arg(exerciseTimer.currentSetRoundNumber).arg(exerciseTimer.currentSetRoundCount))
                        }
                        return parts.join(" · ")
                    }
                }

                Rectangle {
                    width: countDownRect.width - 2 * Theme.horizontalPageMargin
                    height: Theme.itemSizeExtraLarge
                    radius: Theme.paddingSmall
                    color: isWorkout
                           ? UIConstants.workColor
                           : UIConstants.restColor

                    ProgressBar {
                        id: currentProgress
                        //width: countDownRect.width // - 2 * Theme.horizontalPageMargin
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

                Item {

                    width: countDownRect.width - 2 * Theme.horizontalPageMargin
                    height: Theme.itemSizeExtraLarge

                    SummaryBar {
                        id: summaryBar
                        width: parent.width
                        anchors.fill: parent
                        playSequence: exerciseTimer.playSequenceSummary
                        barHeight: parent.height
                    }


                    ProgressBar {
                        id: totalProgress
                        anchors.fill: parent
                        leftMargin: 0
                        rightMargin: 0
                        minimumValue: 0
                        maximumValue: 1
                        enabled: false
                        value: exerciseTimer.totalProgress
                        label: UIConstants.totalProgressLabel
                        valueText: Qt.formatTime(exerciseTimer.totalRunningTime, "hh:mm:ss") + "/" +
                                   Qt.formatTime(exerciseTimer.totalDuration, "hh:mm:ss")

                    }
                }

                RowLayout {

                    id: playControls
                    spacing: Theme.paddingLarge
                    width: currentProgress.width

                    IconButton {
                        icon.source: "image://theme/icon-l-play?" + (pressed
                                  ? Theme.highlightColor
                                  : Theme.primaryColor)
                        Layout.alignment: Qt.AlignLeft
                        enabled: !exerciseTimer.running
                        onClicked: {
                            exerciseTimer.start()
                        }
                    }


                    IconButton {
                        icon.source: "image://theme/icon-l-pause?" + (pressed
                                  ? Theme.highlightColor
                                  : Theme.primaryColor)
                        Layout.alignment: Qt.AlignHCenter
                        enabled: exerciseTimer.running
                        onClicked: {
                            exerciseTimer.pause()
                        }
                     }

                    IconButton {
                        icon.source: "image://theme/icon-m-reload?" + (pressed
                                  ? Theme.highlightColor
                                  : Theme.primaryColor)
                        Layout.alignment: Qt.AlignRight
                        onClicked: {
                            exerciseTimer.reset()
                        }
                    }


            }


            }
        }
    }



}
