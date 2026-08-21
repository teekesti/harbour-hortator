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

    allowedOrientations: Orientation.All

    property var exercise

    property string stagedType: exercise ? exercise.activityType : "work"
    property bool stagedIsWork: stagedType === "work"

    // Filtered list of known exercise names for the autocomplete suggestion row.
    // Re-evaluates whenever nameField.text changes or the library is modified.
    property var filteredNames: {
        var _ = exerciseTimer.templateLibrary.count
        var filter = nameField.text.trim().toLowerCase()
        var all = exerciseTimer.templateLibrary.allNames()
        if (filter === "") return all
        return all.filter(function(n) { return n.toLowerCase().indexOf(filter) >= 0 })
    }

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

        // Name field and suggestion list are grouped with no spacing between
        // them so suggestions appear flush below the input.
        Column {
            width: parent.width
            spacing: 0

            TextField {
                id: nameField
                width: parent.width
                label: qsTr("Exercise name")
                placeholderText: qsTr("Name (optional)")
                text: exercise ? exercise.name : ""
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: dialog.accept()
                rightItem: IconButton {
                    visible: nameField.text.length > 0
                    icon.source: "image://theme/icon-m-clear"
                    onClicked: nameField.text = ""
                }
            }

            // Autocomplete suggestion list: visible while the name field has
            // focus and there is at least one matching history entry. Height
            // is capped so it stays within the area above the virtual keyboard
            // and scrolls internally when there are many matches.
            SilicaListView {
                id: suggestionList
                width: parent.width
                height: Math.min(count * Theme.itemSizeSmall, 4 * Theme.itemSizeSmall)
                clip: true
                visible: nameField.activeFocus && dialog.filteredNames.length > 0
                model: dialog.filteredNames

                delegate: BackgroundItem {
                    width: suggestionList.width
                    height: Theme.itemSizeSmall

                    onClicked: {
                        nameField.text = modelData
                        nameField.focus = false
                    }

                    Label {
                        x: Theme.horizontalPageMargin
                        anchors.verticalCenter: parent.verticalCenter
                        width: parent.width - x - deleteButton.width - Theme.paddingSmall
                        text: modelData
                        elide: Text.ElideRight
                    }

                    IconButton {
                        id: deleteButton
                        anchors {
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
                        icon.source: "image://theme/icon-m-delete"
                        onClicked: exerciseTimer.templateLibrary.removeTemplate(modelData)
                    }
                }

                VerticalScrollDecorator {}
            }
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
