import QtQuick 2.0
import Sailfish.Silica 1.0
import io.github.teekesti.exercisetimer 1.0
import "components"



Page {
    id: listPage

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All
    property int remorseTimeout: 2000 // milliseconds

    // First-use hint sequence (#14, ADR-0019): 0 = inactive, 1-3 = the
    // three hint steps in on-screen order, 4 = finished/cancelled.
    // Started only when exerciseTimer.showFirstUseHints is true (the app
    // just started with an empty Draft, re-seeded with the default
    // Set/Exercise); re-derived fresh on every launch, never persisted.
    property int hintStep: 0

    function cancelHints() {
        if (hintStep > 0 && hintStep < 4) {
            hintStep = 4
        }
    }

    // exerciseTimer isn't registered as a context property yet during
    // this Page's own construction (it's created by main() after the
    // initial QML tree is built - see the similar, pre-existing
    // "exerciseTimer is not defined" Connections warnings from
    // harbour-hortator.qml), so reading it can't happen from
    // Component.onCompleted or an immediate onStatusChanged. Deferring
    // to the next event loop iteration (interval: 0) guarantees main()
    // has already finished registering it by the time this fires.
    Timer {
        id: hintEligibilityTimer
        interval: 0
        onTriggered: {
            if (exerciseTimer.showFirstUseHints) {
                hintStep = 1
            }
        }
    }

    Component.onCompleted: hintEligibilityTimer.start()

    onStatusChanged: {
        if (status === PageStatus.Deactivating) {
            cancelHints()
        }
    }

    function newWorkout() {
        if (exerciseTimer.draftDirty) {
            var dialog = pageStack.push(Qt.resolvedUrl("ConfirmReplaceDraftDialog.qml"))
            dialog.accepted.connect(function() {
                exerciseTimer.resetDraftToDefault()
            })
        } else {
            exerciseTimer.resetDraftToDefault()
        }
    }




    SilicaListView {
        id: setListView
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: toolAndSummaryRow.top
        anchors.bottomMargin: Theme.paddingMedium

        header: PageHeader {
            width: setListView.width
            title: qsTr("Configure exercises")

        }

        model: exerciseListModel

        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
            MenuItem {
                text: qsTr("New workout")
                onClicked: newWorkout()
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }
            MenuItem {
                text: qsTr("History")
                onClicked: pageStack.push(Qt.resolvedUrl("HistoryPage.qml"))
            }
            MenuItem {
                text: qsTr("Exercise names")
                onClicked: pageStack.push(Qt.resolvedUrl("ExerciseLibraryPage.qml"))
            }
            MenuItem {
                text: qsTr("Save workout")
                enabled: exerciseTimer.allValid
                onClicked: pageStack.push(Qt.resolvedUrl("SaveWorkoutDialog.qml"))
            }
        }

        delegate: ListItem {
            id: setItem
            menu: setContextMenu
            contentHeight: setCard.height + Theme.paddingMedium
            ListView.onRemove: animateRemoval(setItem)

            property int setIndex: index

            function removeSet() {
                remorseDelete(function() { exerciseTimer.removeSet(setIndex) }, remorseTimeout)
            }

            SetCard {
                id: setCard
                exerciseSet: set
                setIndex: setItem.setIndex
                highlighted: setItem.highlighted
                width: parent.width
                anchors.top: parent.top
                anchors.topMargin: Theme.paddingSmall

                activateAddHint: listPage.hintStep === 2 && setIndex === 0
                activateExerciseContextMenuHint: listPage.hintStep === 3 && setIndex === 0
                onAddHintFinished: listPage.hintStep = 3
                onContextMenuHintFinished: listPage.hintStep = 4
                onHintInteracted: listPage.cancelHints()
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

    Item {
        id: toolAndSummaryRow
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        height: summaryBar.barHeight + Theme.paddingMedium

        Rectangle {
            anchors.fill: parent
            color: Theme.overlayBackgroundColor
        }

        SummaryBar {
            id: summaryBar
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Theme.paddingMedium
            playSequence: exerciseTimer.playSequenceSummary
            barHeight: buttonRow.height + 2 * Theme.paddingMedium
        }


        Row {
            id: buttonRow
            width: parent.width - 2 * Theme.horizontalPageMargin

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Theme.paddingLarge

            spacing: Theme.paddingMedium


            IconButton {
                id: addSetButton
                icon.source: "image://theme/icon-l-add?" + (pressed
                          ? Theme.highlightColor
                          : Theme.primaryColor)
                onClicked: {
                    exerciseTimer.appendDefaultSet()
                    cancelHints()
                }

                FirstUseHint {
                    anchors.centerIn: parent
                    active: hintStep === 1
                    onFinished: hintStep = 2
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

    InteractionHintLabel {
        // toolAndSummaryRow (and its own background Rectangle) paints
        // above this item despite being declared first - Page apparently
        // doesn't guarantee sibling paint order follows declaration
        // order the way a plain Item would. A high explicit z guarantees
        // this stays on top regardless.
        z: 1000
        // Flips the background gradient's fade direction between steps;
        // no longer affects placement (see anchors.verticalCenter below),
        // which is now fixed regardless of hintStep.
        invert: hintStep === 1
        // A fixed, centered position (rather than anchoring to whichever
        // page edge is farthest from the current hint's target) avoids
        // both overlapping the target control's own text/controls and
        // being clipped at the bottom edge, which the previous
        // per-step top/bottom placement suffered from.
        anchors.verticalCenter: parent.verticalCenter
        visible: hintStep > 0 && hintStep < 4
        text: {
            switch (hintStep) {
            case 1: return qsTr("Tap to add a new set")
            case 2: return qsTr("Tap to add an exercise, press and hold to pick a saved one")
            case 3: return qsTr("Press and hold to edit or remove")
            default: return ""
            }
        }
        bottomMargin: Theme.paddingLarge
        topMargin: Theme.paddingLarge
        backgroundColor: Theme.backgroundGlowColor
    }





}
