import QtQuick 2.0
import Sailfish.Silica 1.0

// Lists exercise templates (ADR-0008) and lets the user delete ones they
// no longer want lingering (ADR-0009). Templates are otherwise managed
// implicitly, by naming exercises in the editor.
Page {
    id: page

    allowedOrientations: Orientation.All
    property int remorseTimeout: 2000 // milliseconds

    SilicaListView {
        anchors.fill: parent
        model: exerciseTimer.templateLibrary

        header: PageHeader {
            title: qsTr("Exercise names")
        }

        ViewPlaceholder {
            enabled: exerciseTimer.templateLibrary.count === 0
            text: qsTr("No saved exercise names yet")
            hintText: qsTr("Name an exercise in the editor to see it here")
        }

        delegate: ListItem {
            id: templateItem
            contentHeight: nameLabel.height + 2 * Theme.paddingMedium
            ListView.onRemove: animateRemoval(templateItem)

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Delete")
                    onClicked: remorseDelete(function() {
                        exerciseTimer.templateLibrary.removeTemplate(model.name)
                    }, remorseTimeout)
                }
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
