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

    backNavigation: !exerciseTimer.running && !exerciseTimer.paused && !exerciseTimer.waitingToStart

    property bool _cancelConfirmed: false
    property bool isWorkout: exerciseTimer.currentActivity.activityType === "work"

    function remainingSecs(duration, elapsed) {
        var d = duration.getUTCHours() * 3600 + duration.getUTCMinutes() * 60 + duration.getUTCSeconds()
        var e = elapsed.getUTCHours() * 3600 + elapsed.getUTCMinutes() * 60 + elapsed.getUTCSeconds()
        return Math.max(0, d - e)
    }

    function formatSecs(secs) {
        var h = Math.floor(secs / 3600)
        var m = Math.floor((secs % 3600) / 60)
        var s = secs % 60
        var ss = s < 10 ? "0" + s : s
        if (h > 0) {
            var mm = m < 10 ? "0" + m : m
            return h + ":" + mm + ":" + ss
        }
        return m + ":" + ss
    }

    onStatusChanged: {
        if (status === PageStatus.Active && _cancelConfirmed) {
            _cancelConfirmed = false
            pageStack.pop()
        }
    }

    DisplayBlanking {
            preventBlanking: runPage.status === PageStatus.Active && exerciseTimer.running
        }

    KeepAlive {
        // Keeps CPU awake so background audio/timers work if screen turns off
        enabled: exerciseTimer.running
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: parent.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Cancel workout")
                onClicked: {
                    var dialog = pageStack.push(Qt.resolvedUrl("ConfirmCancelWorkoutDialog.qml"))
                    dialog.accepted.connect(function() {
                        exerciseTimer.reset()
                        _cancelConfirmed = true
                    })
                }
            }
        }

        Flow {
        flow: orientation == Orientation.Portrait ? Flow.TopToBottom : Flow.LeftToRight
        anchors.fill: parent

        Rectangle {
            id: countDownRect
            width: orientation == Orientation.Portrait ? runPage.width : Math.min(runPage.width / 2, runPage.height)
            height: width
            color: Theme.backgroundGlowColor

            property bool isCountingDown: false
            property int countdownDigit: 0

            Label {
                id: countDownLabel
                anchors.centerIn: parent
                width: parent.width
                color: Theme.highlightColor
                style: Text.Raised
                styleColor: isWorkout ? UIConstants.workColor : UIConstants.restColor
                font.bold: true
                font.pixelSize: parent.height * 0.8
                fontSizeMode: Text.Fit
                minimumPixelSize: Theme.fontSizeExtraLarge
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                opacity: 1
                text: countDownRect.isCountingDown
                      ? countDownRect.countdownDigit
                      : runPage.formatSecs(runPage.remainingSecs(exerciseTimer.currentDuration,
                                                                 exerciseTimer.currentRunningTime))

                SequentialAnimation {
                    id: flashAnimation
                    NumberAnimation { target: countDownLabel; property: "opacity"; easing.type: Easing.InOutExpo; to: 60; duration: 200 }
                    NumberAnimation { target: countDownLabel; property: "opacity"; easing.type: Easing.InOutExpo; to: 0; duration: 500 }
                }
            }

            Connections {
                target: exerciseTimer
                onCountDown: {
                    if (number > 0) {
                        flashAnimation.stop()
                        countDownRect.countdownDigit = number
                        countDownRect.isCountingDown = true
                        countDownLabel.opacity = 0
                        flashAnimation.start()
                    } else {
                        flashAnimation.stop()
                        countDownRect.isCountingDown = false
                        countDownLabel.opacity = 1
                    }
                }
            }


        }

        Item {
            width:  orientation == Orientation.Portrait ? runPage.width : runPage.width - countDownRect.width
            height: orientation == Orientation.Portrait ? runPage.height - countDownRect.height : runPage.height

            Column {
                anchors.fill: parent
                anchors.margins: Theme.paddingLarge
                spacing: Theme.paddingLarge

                property real itemWidth : parent.width - 2 * Theme.paddingLarge

                Column {
                    width: parent.itemWidth

                    Label {
                        truncationMode: TruncationMode.Fade
                        font.pixelSize: Theme.fontSizeLarge
                        font.bold: true
                        color: Theme.secondaryColor
                        text: {
                            var exerciseName = exerciseTimer.currentActivity.name
                            return exerciseName ? exerciseName : isWorkout ? qsTr("Work") : qsTr("Rest")
                        }
                    }

                    Label {
                        id: positionLabel
                        font.pixelSize: Theme.fontSizeLarge
                        font.bold: true
                        color: Theme.secondaryColor
                        visible: text.length > 0
                        text: {
                            var parts = []
                            if (exerciseTimer.currentSetCount > 1) {
                                parts.push(qsTr("Set %1/%2").arg(exerciseTimer.currentSetNumber).arg(exerciseTimer.currentSetCount))
                            }
                            if (exerciseTimer.currentSetRoundCount > 1) {
                                parts.push(qsTr("Set round %1/%2").arg(exerciseTimer.currentSetRoundNumber).arg(exerciseTimer.currentSetRoundCount))
                            }
                            return parts.join(" · ")
                        }
                    }
                }



                ActivityProgressBar {
                    width: parent.itemWidth
                }

                TotalProgressBar {
                    width: parent.itemWidth
                }

                Label {
                    width: parent.itemWidth
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Theme.fontSizeLarge
                    color: Theme.secondaryColor
                    //visible: orientation == Orientation.Portrait
                    text: qsTr("Workout remaining: %1").arg(
                              runPage.formatSecs(runPage.remainingSecs(exerciseTimer.totalDuration,
                                                                       exerciseTimer.totalRunningTime)))
                }

                RowLayout {

                    id: playControls
                    spacing: Theme.paddingLarge
                    width: parent.itemWidth

                    IconButton {
                        icon.source: "image://theme/icon-l-play?" + (pressed
                                  ? Theme.highlightColor
                                  : Theme.primaryColor)
                        Layout.alignment: Qt.AlignLeft
                        enabled: exerciseTimer.paused
                                 || (!exerciseTimer.running && !exerciseTimer.waitingToStart)
                        onClicked: {
                            exerciseTimer.start()
                        }
                    }


                    IconButton {
                        icon.source: "image://theme/icon-l-pause?" + (pressed
                                  ? Theme.highlightColor
                                  : Theme.primaryColor)
                        Layout.alignment: Qt.AlignHCenter
                        enabled: (exerciseTimer.running || exerciseTimer.waitingToStart)
                                 && !exerciseTimer.paused
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
        } // Flow
    } // SilicaFlickable
}

