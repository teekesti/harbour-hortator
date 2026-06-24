import QtQuick 2.0
import Sailfish.Silica 1.0

// Names (or renames) a single Exercise. The name is only written back to
// the C++ engine on acceptance, not on every keystroke (ADR-0010); naming
// upserts a template by that name (ADR-0008/0009).
Dialog {
    id: dialog

    property var exercise
    property alias exerciseName: nameField.text

    onAccepted: {
        if (exercise) {
            exercise.name = exerciseName.trim()
        }
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
    }
}
