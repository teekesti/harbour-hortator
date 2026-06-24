import QtQuick 2.0
import Sailfish.Silica 1.0
import com.appiukko.exercisetimer 1.0
import "components"



Page {
    id: listPage

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All
    property int remorseTimeout: 2000 // milliseconds




    SilicaListView {
        id: setListView
        width: parent.width // - margin?

        anchors.top: parent.top
        anchors.bottom: buttonRow.top
        anchors.bottomMargin: Theme.paddingMedium

        header: PageHeader {
            width: setListView.width
            title: qsTr("Configure exercises")

        }

        model: exerciseListModel
        delegate: ListItem {
            id: setItem
            menu: setContextMenu
            contentHeight: setColumn.height + Theme.paddingMedium
            ListView.onRemove: animateRemoval(setItem)

            // Captured so nested Repeater delegates (which have their own
            // "index") can still refer to this set's position in the
            // top-level list.
            property int setIndex: index

            function removeSet() {
                remorseDelete(function() { exerciseTimer.removeSet(setIndex) }, remorseTimeout)
            }

            Column {
                id: setColumn
                width: parent.width
                anchors.top: parent.top
                anchors.topMargin: Theme.paddingSmall
                spacing: Theme.paddingSmall

                Rectangle {
                    id: setHeaderBackground
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: setHeaderRow.height + Theme.paddingMedium
                    radius: Theme.paddingSmall

                    color: {
                        if (!set || !set.isValid) {
                            return Theme.errorColor
                        }
                        if (setItem.highlighted) {
                            return Theme.rgba(Theme.highlightColor, Theme.opacityLow)
                        }
                        return Theme.rgba(Theme.highlightBackgroundColor, 0.4)
                    }

                    Row {
                        id: setHeaderRow
                        anchors {
                            left: parent.left; right: parent.right
                            verticalCenter: parent.verticalCenter
                            leftMargin: Theme.horizontalPageMargin
                            rightMargin: Theme.horizontalPageMargin
                        }
                        spacing: Theme.paddingMedium

                        Label {
                            text: qsTr("Set %1").arg(setIndex + 1)
                            font.bold: true
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        RoundCountAdjustment {
                            id: setRoundsAdjustment
                            value: set ? set.rounds : 1
                            minValue: 1
                            maxValue: 99
                        }
                        Binding { target: set; property: "rounds"; value: setRoundsAdjustment.value }

                        IconButton {
                            anchors.verticalCenter: parent.verticalCenter
                            icon.source: "image://theme/icon-m-add?" + (pressed
                                      ? Theme.highlightColor
                                      : Theme.primaryColor)
                            onClicked: exerciseTimer.addDefaultExerciseToSet(setIndex)
                        }
                    }
                }

                Repeater {
                    id: exerciseRepeater
                    width: parent.width
                    model: set ? set.count : 0

                    delegate: Item {
                        id: exerciseRow
                        width: exerciseRepeater.width
                        height: exerciseContentRow.height

                        property int exerciseIndex: index
                        property var exercise: set ? set.at(exerciseIndex) : null
                        property bool isWorkout: exercise ? exercise.activityType === "work" : false

                        Rectangle {
                            height: setHeaderBackground.height
                            anchors { fill: parent; margins: Theme.paddingSmall }
                            radius: Theme.paddingSmall

                            color: {
                                if (!exercise || !exercise.isValid) {
                                    return Theme.errorColor
                                }
                                return isWorkout
                                        ? Theme.rgba(Theme.highlightBackgroundColor, 0.25)
                                        : Theme.rgba(Theme.highlightDimmerColor, 0.10)
                            }

                            Row {
                                id: exerciseContentRow
                                anchors {
                                    left: parent.left; right: parent.right
                                    leftMargin: Theme.horizontalPageMargin
                                    rightMargin: Theme.horizontalPageMargin
                                }
                                spacing: Theme.paddingMedium


                                Flow {
                                    flow: orientation == Orientation.Portrait ? Flow.TopToBottom : Flow.LeftToRight
                                    anchors.verticalCenter: parent.verticalCenter

                                    Button {
                                        id: activityTypeButton
                                        text: isWorkout ? qsTr("Work") : qsTr("Rest")
                                        color: isWorkout ? Theme.primaryColor : Theme.secondaryColor
                                        onClicked: if (exercise) exercise.toggleActivityType()
                                    }

                                    RoundCountAdjustment {
                                        id: roundsAdjustment
                                        value: exercise ? exercise.rounds : 1
                                        minValue: 1
                                        maxValue: 99
                                    }

                                    Binding { target: exercise; property: "rounds"; value: roundsAdjustment.value }
                                }



                                Flow {
                                    id: timeAdjustment
                                    flow: orientation == Orientation.Portrait ? Flow.TopToBottom : Flow.LeftToRight
                                    spacing: 0.5 * Theme.paddingSmall
                                    anchors.verticalCenter: parent.verticalCenter

                                    ValueAdjustmentHorizontal {
                                        id: minutesAdjustment
                                        value: exercise ? exercise.mins : 0
                                        maxValue: 99
                                        unitLabel: "m"
                                    }

                                    ValueAdjustmentHorizontal {
                                        id: secondsAdjustment
                                        value: exercise ? exercise.secs : 0
                                        maxValue: 59
                                        unitLabel: "s"
                                        step: 5
                                    }


                                    Binding { target: exercise; property: "mins"; value: minutesAdjustment.value }
                                    Binding { target: exercise; property: "secs"; value: secondsAdjustment.value }   

                                }

                                IconButton {
                                    anchors.verticalCenter: parent.verticalCenter
                                    icon.source: "image://theme/icon-m-delete?" + (pressed
                                              ? Theme.highlightColor
                                              : Theme.primaryColor)
                                    onClicked: exerciseTimer.removeExerciseFromSet(setItem.setIndex, exerciseIndex)
                                }
                            }
                        }
                    }
                }
            }

            Component {
                id: setContextMenu
                ContextMenu {
                    MenuItem {
                        text: qsTr("Remove set")
                        onClicked: removeSet()
                    }
                }
            }


        }
    }

    Row {
        id: buttonRow
        width: parent.width - 2 * Theme.horizontalPageMargin

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.paddingLarge

        spacing: Theme.paddingMedium

        IconButton {
            icon.source: "image://theme/icon-l-add?" + (pressed
                      ? Theme.highlightColor
                      : Theme.primaryColor)
            onClicked: {
                exerciseTimer.appendDefaultSet()
            }
         }

        IconButton {
            icon.source: "image://theme/icon-l-play?" + (pressed
                      ? Theme.highlightColor
                      : Theme.primaryColor)
            enabled: exerciseTimer.allValid
            onClicked: {
                exerciseTimer.start()
                pageStack.animatorPush("RunPage.qml")
            }
         }

        Label {
            property string labelPrefix: orientation == Orientation.Portrait ? qsTr("Total") : qsTr("Total Duration")
            text: labelPrefix + ": " + Qt.formatTime(exerciseTimer.totalDuration, "hh:mm:ss")
            anchors.verticalCenter: parent.verticalCenter
        }
    }





}
