import QtQuick 2.0
import Sailfish.Silica 1.0
import "components"

// Lists saved workouts (ADR-0005/0006), most-recently-played first
// (ADR-0015). Tapping a row plays it directly; its context menu offers
// opening it in the editor instead, or deleting it (ADR-0014). Both play
// and open replace the Draft, confirming first if it has unsaved changes
// (ADR-0011).
Page {
    id: historyPage

    property int remorseTimeout: 2000 // milliseconds

    function withDraftConfirmation(action) {
        if (exerciseTimer.draftDirty) {
            var dialog = pageStack.push(Qt.resolvedUrl("ConfirmReplaceDraftDialog.qml"))
            dialog.accepted.connect(action)
        } else {
            action()
        }
    }

    function playEntry(entryId) {
        withDraftConfirmation(function() {
            exerciseTimer.loadHistoryEntry(entryId)
            exerciseTimer.history.markPlayed(entryId)
            exerciseTimer.start()
            pageStack.animatorPush(Qt.resolvedUrl("RunPage.qml"))
        })
    }

    function openEntry(entryId) {
        withDraftConfirmation(function() {
            exerciseTimer.loadHistoryEntry(entryId)
            pageStack.pop()
        })
    }

    SilicaListView {
        id: historyListView
        anchors.fill: parent
        model: exerciseTimer.history

        header: PageHeader {
            width: historyListView.width
            title: qsTr("History")
        }

        ViewPlaceholder {
            enabled: exerciseTimer.history.count === 0
            text: qsTr("No saved workouts yet")
            hintText: qsTr("Save a valid workout from the pull-down menu to see it here")
        }

        delegate: ListItem {
            id: historyItem
            contentHeight: historyColumn.height + Theme.paddingMedium
            ListView.onRemove: animateRemoval(historyItem)

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Open in editor")
                    onClicked: openEntry(model.entryId)
                }
                MenuItem {
                    text: qsTr("Delete")
                    onClicked: remorseDelete(function() {
                        exerciseTimer.history.removeEntry(model.entryId)
                    }, remorseTimeout)
                }
            }

            onClicked: playEntry(model.entryId)

            Column {
                id: historyColumn
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                spacing: Theme.paddingSmall

                Label {
                    x: Theme.horizontalPageMargin
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    text: model.name
                    font.bold: true
                    truncationMode: TruncationMode.Fade
                }

                Label {
                    x: Theme.horizontalPageMargin
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    text: model.summary
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryColor
                    truncationMode: TruncationMode.Fade
                }

                SummaryBar {
                    x: Theme.horizontalPageMargin
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    playSequence: model.playSequence
                }
            }
        }
        VerticalScrollDecorator {}
    }
}
