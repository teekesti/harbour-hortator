import QtQuick 2.0
import Sailfish.Silica 1.0

// Names and saves a snapshot of the current Draft into History
// (ADR-0006/0007). Only reachable when the Draft is valid - the caller
// gates this (ADR-0012).
Dialog {
    id: dialog

    property alias workoutName: nameField.text

    canAccept: workoutName.trim().length > 0

    onAccepted: {
        exerciseTimer.saveDraftToHistory(workoutName.trim())
    }

    Column {
        width: parent.width
        spacing: Theme.paddingLarge

        DialogHeader {
            acceptText: qsTr("Save")
        }

        TextField {
            id: nameField
            width: parent.width
            label: qsTr("Workout name")
            text: Qt.formatDate(new Date(), "yyyy-MM-dd")
            EnterKey.enabled: dialog.canAccept
            EnterKey.iconSource: "image://theme/icon-m-enter-accept"
            EnterKey.onClicked: dialog.accept()
        }
    }
}
