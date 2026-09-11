import QtQuick 2.0
import Sailfish.Silica 1.0
import io.github.teekesti.exercisetimer 1.0
import "."

ListItem {
    id: exerciseRow

    property int exerciseIndex
    property int setIndex
    property var exercise: null

    property bool isWorkout: exercise ? exercise.activityType === "work" : false

    // First-use hint (#14, ADR-0019): points out the press-and-hold
    // context menu below. activateContextMenuHint is driven by the page
    // orchestrating the hint sequence; this row just reports back.
    property bool activateContextMenuHint: false
    signal contextMenuHintFinished()
    signal hintInteracted()

    onMenuOpenChanged: if (menuOpen) hintInteracted()

    contentHeight: exerciseContentColumn.height

    menu: ContextMenu {
        MenuItem {
            text: qsTr("Edit exercise")
            onClicked: pageStack.push(Qt.resolvedUrl("../EditExerciseDialog.qml"),
                                       {exercise: exercise})
        }
        MenuItem {
            text: qsTr("Remove exercise")
            onClicked: exerciseTimer.removeExerciseFromSet(setIndex, exerciseIndex)
        }
    }

    Rectangle {
        anchors { fill: parent}
        radius: Theme.paddingSmall

        color: {
            if (!exercise || !exercise.isValid) {
                return Theme.errorColor
            }
            return isWorkout
                    ? UIConstants.workColor
                    : UIConstants.restColor
        }

        Column {
            id: exerciseContentColumn
            anchors {
                left: parent.left; right: parent.right
                leftMargin: Theme.horizontalPageMargin
                rightMargin: Theme.horizontalPageMargin
            }
            spacing: 0

            Label {
                width: parent.width
                visible: orientation === Orientation.Portrait && exercise && exercise.name.length > 0
                text: exercise ? exercise.name : ""
                truncationMode: TruncationMode.Fade
            }

            Item {
                id: exerciseContentRow
                width: parent.width
                height: controlsRow.height

                Row {
                    id: controlsRow
                    spacing: Theme.paddingMedium
                    anchors.verticalCenter: parent.verticalCenter

                    Flow {
                        flow: orientation == Orientation.Portrait ? Flow.TopToBottom : Flow.LeftToRight
                        anchors.verticalCenter: parent.verticalCenter

                        Button {
                            id: activityTypeButton
                            width: roundsAdjustment.width
                            text: isWorkout ? qsTr("Work") : qsTr("Rest")
                            color: isWorkout ? Theme.primaryColor : Theme.secondaryColor
                            onClicked: if (exercise) exercise.toggleActivityType()
                        }

                        RoundCountAdjustment {
                            id: roundsAdjustment
                            height: activityTypeButton.height
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
                            height: activityTypeButton.height
                            value: exercise ? exercise.mins : 0
                            maxValue: 99
                            unitLabel: "m"
                        }

                        ValueAdjustmentHorizontal {
                            id: secondsAdjustment
                            height: activityTypeButton.height
                            width: minutesAdjustment.width
                            value: exercise ? exercise.secs : 0
                            maxValue: 59
                            unitLabel: "s"
                            step: 5
                        }

                        Binding { target: exercise; property: "mins"; value: minutesAdjustment.value }
                        Binding { target: exercise; property: "secs"; value: secondsAdjustment.value }
                    }
                }

                Label {
                    visible: orientation !== Orientation.Portrait && exercise && exercise.name.length > 0
                    text: exercise ? exercise.name : ""
                    anchors {
                        left: controlsRow.right
                        leftMargin: Theme.paddingMedium
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    truncationMode: TruncationMode.Fade
                }
            }
        }
    }

    FirstUseHint {
        anchors.centerIn: parent
        active: activateContextMenuHint
        onFinished: exerciseRow.contextMenuHintFinished()
    }
}
