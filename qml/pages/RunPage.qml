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
            width: orientation == Orientation.Portrait ? runPage.width : runPage.width / 2
            height: width
            color: Theme.backgroundGlowColor

            Label { // Display a large countdown number
                    id: countdownNumber
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
                            currentRemainingTimeLabel.opacity = 0
                        }
                        else
                        {
                            countdownNumber.opacity = 0
                            currentRemainingTimeLabel.opacity = 1

                        }
                    }
                }

            Label {
                id: currentRemainingTimeLabel
                anchors.fill: parent
                color: Theme.highlightColor
                font.bold: true
                style: Text.Raised
                font.pixelSize: parent.height * 0.8
                fontSizeMode: Text.Fit
                minimumPixelSize: Theme.fontSizeExtraLarge
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                opacity: flashCountdownNumber.running ? 0 : 1
                text: runPage.formatSecs(runPage.remainingSecs(exerciseTimer.currentDuration,
                                                               exerciseTimer.currentRunningTime))
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

                ActivityProgressBar {
                    width: countDownRect.width - 2 * Theme.horizontalPageMargin
                }

                TotalProgressBar {
                    width: countDownRect.width - 2 * Theme.horizontalPageMargin
                }

                Label {
                    width: countDownRect.width - 2 * Theme.horizontalPageMargin
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.secondaryColor
                    visible: orientation == Orientation.Portrait
                    text: qsTr("Workout remaining: %1").arg(
                              runPage.formatSecs(runPage.remainingSecs(exerciseTimer.totalDuration,
                                                                       exerciseTimer.totalRunningTime)))
                }

                RowLayout {

                    id: playControls
                    spacing: Theme.paddingLarge
                    width: countDownRect.width - 2 * Theme.horizontalPageMargin

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
        } // Flow
    } // SilicaFlickable
}

