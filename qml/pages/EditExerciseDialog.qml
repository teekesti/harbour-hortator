import QtQuick 2.0
import Sailfish.Silica 1.0
import "components"

// Edits a single Exercise: name, work/rest type, duration, rounds, and
// reps. Every field is staged locally and written back to the C++ engine
// only on acceptance (ADR-0018); backing out discards all staged changes.
// The inline row controls in FirstPage.qml are unaffected - they keep
// writing live, as an alternate path to the same fields.
Dialog {
    id: dialog

    property var exercise

    property string stagedType: exercise ? exercise.activityType : "work"
    property bool stagedIsWork: stagedType === "work"

    onAccepted: {
        if (!exercise) {
            return
        }
        exercise.name = nameField.text.trim()
        exercise.activityType = stagedType
        exercise.mins = minutesAdjustment.value
        exercise.secs = secondsAdjustment.value
        exercise.rounds = roundsAdjustment.value
        exercise.reps = repsAdjustment.value
    }

    Column {
        width: parent.width
        spacing: Theme.paddingLarge

        DialogHeader {
            acceptText: qsTr("Done")
        }

        TextField {
            id: nameField
            width: parent.width
            label: qsTr("Exercise name")
            placeholderText: qsTr("Name (optional)")
            text: exercise ? exercise.name : ""
            EnterKey.iconSource: "image://theme/icon-m-enter-accept"
            EnterKey.onClicked: dialog.accept()
        }

        Row {
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * Theme.horizontalPageMargin
            spacing: Theme.paddingMedium

            Button {
                text: dialog.stagedIsWork ? qsTr("Work") : qsTr("Rest")
                color: dialog.stagedIsWork ? Theme.primaryColor : Theme.secondaryColor
                onClicked: dialog.stagedType = dialog.stagedIsWork ? "rest" : "work"
            }

            RoundCountAdjustment {
                id: roundsAdjustment
                anchors.verticalCenter: parent.verticalCenter
                value: exercise ? exercise.rounds : 1
                minValue: 1
                maxValue: 99
            }
        }

        Row {
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * Theme.horizontalPageMargin
            spacing: 0.5 * Theme.paddingSmall

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
        }

        ValueAdjustmentHorizontal {
            id: repsAdjustment
            x: Theme.horizontalPageMargin
            visible: dialog.stagedIsWork
            value: exercise ? exercise.reps : 0
            maxValue: 99
            unitLabel: qsTr("reps")
        }
    }
}
