import QtQuick 2.0
import Sailfish.Silica 1.0
import io.github.teekesti.exercisetimer 1.0
import "components"



Page {
    id: listPage

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All
    property int remorseTimeout: 2000 // milliseconds

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







}
