import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: runpage

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    Flow {
        flow: orientation == Orientation.Portrait ? Flow.TopToBottom : Flow.LeftToRight
        anchors.fill: parent

        Rectangle {
            id: countDownRect
            //anchors.horizontalCenter: parent.horizontalCenter
            width: orientation == Orientation.Portrait ? runpage.width : runpage.width / 2
            height: width
            color: Theme.backgroundGlowColor

            Text { // Display a large countdown number
                    id: countdownNumber
                    anchors.centerIn: parent
                    color: "#f10ed3"
                    font.bold: true
                    style: Text.Raised
                    font.pixelSize: 360
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


            ProgressBar {
                id: currentProgress
                width: countDownRect.width // - 2 * Theme.horizontalPageMargin
                //anchors.top: parent.top
                minimumValue: 0
                maximumValue: 1
                enabled: false
                value: exerciseTimer.currentProgress
                label: qsTr("Current progress")
                valueText: Qt.formatTime(exerciseTimer.currentRunningTime, "mm:ss") + "/" +
                           Qt.formatTime(exerciseTimer.currentDuration, "mm:ss")

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

            Row {
                id: playControls
                width: currentProgress.width

                anchors.horizontalCenter: parent.horizontalCenter

                spacing: Theme.paddingLarge
                IconButton {
                    icon.source: "image://theme/icon-l-play?" + (pressed
                              ? Theme.highlightColor
                              : Theme.primaryColor)
                    enabled: !exerciseTimer.running
                    onClicked: {
                        exerciseTimer.start()
                    }
                }

                IconButton {
                    icon.source: "image://theme/icon-l-pause?" + (pressed
                              ? Theme.highlightColor
                              : Theme.primaryColor)
                    enabled: exerciseTimer.running
                    onClicked: {
                        exerciseTimer.pause()
                    }
                 }

            }
        }
    }



}
