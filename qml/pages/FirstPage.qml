import QtQuick 2.0
import Sailfish.Silica 1.0
import com.appiukko.exercisetimer 1.0

Page {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All


    SilicaListView {
        id: exerciseListView
        width: parent.width // - margin?

        anchors.top: parent.top
        anchors.bottom: buttonRow.top
        anchors.bottomMargin: Theme.paddingMedium

        header: PageHeader {
            title: "Configure exercises"
        }

        model: exerciseListModel
        delegate: ListItem {
            id: exerciseItem
            menu: contextMenu
            contentHeight: Theme.itemSizeMedium
            ListView.onRemove: animateRemoval(exerciseItem)

            function remove() {
                remorseAction("Deleting", function() { exerciseTimer.removeExercise(index) })
            }

            Rectangle{
                anchors {fill: parent; margins: Theme.paddingSmall}
                radius: Theme.paddingSmall

                color: {
                    if (exerciseItem.highlighted) {
                        return Theme.rgba(Theme.highlightColor, Theme.opacityLow)
                    }

                    return exercise.activityType === "work"
                                ? Theme.rgba(Theme.highlightBackgroundColor, 0.25)
                                : Theme.rgba(Theme.highlightDimmerColor)
            }

            Row {
                anchors {fill: parent; leftMargin: Theme.horizontalPageMargin; rightMargin: Theme.horizontalPageMargin}
                spacing: Theme.paddingMedium

                Label {
                    id: workRestLabel
                    text: exercise.activityType
                    anchors.verticalCenter: parent.verticalCenter
                    color: exercise.activityType === "work" ? Theme.primaryColor : Theme.secondaryColor
                    font.bold: exercise.activityType === "work"
                }

                //Spacer {id: fillSpace}

                Label {
                    text: exercise.mins + "m" + exercise.secs + "s"
                    anchors.verticalCenter: parent.verticalCenter
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeMedium
                }
                IconButton {
                    icon.source: "image://theme/icon-m-delete?" + (pressed
                              ? Theme.highlightColor
                              : Theme.primaryColor)
                    onClicked: remove()
                }
            }



            Component {
                id: contextMenu
                ContextMenu {
                    MenuItem {
                        text: "Remove"
                        onClicked: remove()
                    }
                }
            }
        }
    }
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
                exerciseTimer.appendDefaultExercise()
            }
         }
    }





}

