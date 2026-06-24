import QtQuick 2.0
import Sailfish.Silica 1.0

// Lets the user add an Exercise to a Set either blank or copied (by
// value) from an existing named template (ADR-0008).
Page {
    id: page

    property int setIndex: -1

    SilicaListView {
        anchors.fill: parent
        model: exerciseTimer.templateLibrary

        header: Column {
            width: parent.width

            PageHeader {
                title: qsTr("Add exercise")
            }

            ListItem {
                contentHeight: blankLabel.height + 2 * Theme.paddingMedium
                onClicked: {
                    exerciseTimer.addDefaultExerciseToSet(setIndex)
                    pageStack.pop()
                }

                Label {
                    id: blankLabel
                    x: Theme.horizontalPageMargin
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Blank")
                }
            }
        }

        ViewPlaceholder {
            enabled: exerciseTimer.templateLibrary.count === 0
            text: qsTr("No saved exercise names yet")
            hintText: qsTr("Name an exercise in the editor to reuse it here later")
        }

        delegate: ListItem {
            contentHeight: nameLabel.height + 2 * Theme.paddingMedium
            onClicked: {
                exerciseTimer.addExerciseToSetFromTemplate(setIndex, model.name)
                pageStack.pop()
            }

            Label {
                id: nameLabel
                x: Theme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter
                text: model.name
            }
        }
        VerticalScrollDecorator {}
    }
}
