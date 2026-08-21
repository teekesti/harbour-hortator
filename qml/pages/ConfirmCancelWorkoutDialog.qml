import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    id: dialog

    acceptDestinationAction: PageStackAction.Pop

    Column {
        width: parent.width
        spacing: Theme.paddingLarge

        DialogHeader {
            acceptText: qsTr("Cancel workout")
            cancelText: qsTr("Continue")
        }

        Label {
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * Theme.horizontalPageMargin
            wrapMode: Text.Wrap
            text: qsTr("Cancel the current workout? Your progress will be lost.")
        }
    }
}
