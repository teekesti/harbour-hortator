import QtQuick 2.0
import Sailfish.Silica 1.0

// Confirms replacing the current Draft with a History entry when the
// Draft has unsaved changes (ADR-0011). Pushed only when
// exerciseTimer.draftDirty is true - the caller skips straight to the
// replacement otherwise.
Dialog {
    id: dialog

    acceptDestinationAction: PageStackAction.Pop

    Column {
        width: parent.width
        spacing: Theme.paddingLarge

        DialogHeader {
            acceptText: qsTr("Replace")
        }

        Label {
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * Theme.horizontalPageMargin
            wrapMode: Text.Wrap
            text: qsTr("Discard your unsaved changes to the current workout?")
        }
    }
}
