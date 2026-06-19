import QtQuick 2.0
import Sailfish.Silica 1.0
import QtQuick.Layouts 1.1 // Required for RowLayout
import Nemo.KeepAlive 1.2 // To prevent screen blanking

Page {
    id: runPage

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All
    property bool isWorkout: exerciseTimer.currentActivity.activityType === "work"

    DisplayBlanking {
            // Keeps screen awake ONLY when page is active AND timer is ticking
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
                    // Disabled animations because they ate too much CPU from sounds
                    //SequentialAnimation  {
                    // id: flashCountdownNumber
                    //            NumberAnimation {id: showAnimation; target: countdownNumber;
                    //                property: "opacity"; easing.type: Easing.InOutExpo; to: 60; duration: 200 }
                    //            NumberAnimation {id: hideAnimation; target: countdownNumber;
                    //                property: "opacity"; easing.type: Easing.InOutExpo; to: 0; duration: 500 }
                    //        //}

                }

                Connections {
                    target: exerciseTimer
                    onCountDown: {
                        if (number > 0)
                        {
                            countdownNumber.text = number // number from signal parameter
                            countdownNumber.opacity = 60
            //                playButton.enabled = false
            //                resetButton.enabled = false
            //                backToListButton.enabled = false
            //                settingsButton.enabled = false
                        }
                        else
                        {
                            // hide the number when countdown is ready
                            countdownNumber.opacity = 0
            //                playButton.enabled = true
            //                resetButton.enabled = true
            //                backToListButton.enabled = !exerciseTimer.running
            //                settingsButton.enabled = true
                        }
                    }
                }

        }

        Column {
            spacing: Theme.paddingMedium

            Rectangle {
                width: countDownRect.width - 2 * Theme.horizontalPageMargin
                height: Theme.itemSizeExtraLarge
                radius: Theme.paddingSmall
                color: isWorkout
                       ? Theme.rgba(Theme.highlightBackgroundColor, 0.25)
                       : Theme.rgba(Theme.highlightDimmerColor, 0.10)

                ProgressBar {
                    id: currentProgress
                    //width: countDownRect.width // - 2 * Theme.horizontalPageMargin
                    anchors.fill: parent
                    minimumValue: 0
                    maximumValue: 1
                    enabled: false
                    value: exerciseTimer.currentProgress
                    label: qsTr("Current progress")
                    valueText: Qt.formatTime(exerciseTimer.currentRunningTime, "mm:ss") + "/" +
                               Qt.formatTime(exerciseTimer.currentDuration, "mm:ss")


                }

            }



            ProgressBar {
                id: totalProgress
                width: currentProgress.width
                minimumValue: 0
                maximumValue: 1
                enabled: false
                value: exerciseTimer.totalProgress
                label: qsTr("Total progress")
                valueText: Qt.formatTime(exerciseTimer.totalRunningTime, "hh:mm:ss") + "/" +
                           Qt.formatTime(exerciseTimer.totalDuration, "hh:mm:ss")

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
