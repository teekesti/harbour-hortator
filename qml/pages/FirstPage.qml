import QtQuick 2.0
import Sailfish.Silica 1.0
import com.appiukko.exercisetimer 1.0
import "components"



Page {
    id: listPage

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All
    property int remorseTimeout: 2000 // milliseconds




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
            contentHeight: timeAdjustment.height
            ListView.onRemove: animateRemoval(exerciseItem)

            property bool isWorkout: exercise.activityType === "work"

            function remove() {
                remorseDelete(function() { exerciseTimer.removeExercise(index) }, remorseTimeout)
            }





            Rectangle{
                anchors {fill: parent; margins: Theme.paddingSmall}
                radius: Theme.paddingSmall

                color: {
                    if (exerciseItem.highlighted) {
                        return Theme.rgba(Theme.highlightColor, Theme.opacityLow)
                    }

                    return isWorkout
                            ? Theme.rgba(Theme.highlightBackgroundColor, 0.25)
                            : Theme.rgba(Theme.highlightDimmerColor, 0.10)
                }

            Row {
                anchors {fill: parent; leftMargin: Theme.horizontalPageMargin; rightMargin: Theme.horizontalPageMargin}
                spacing: Theme.paddingMedium

                Button {
                    text: exercise.activityType
                    anchors.verticalCenter: parent.verticalCenter
                    color: isWorkout ? Theme.primaryColor : Theme.secondaryColor
                    //font.bold: exercise.activityType === "work"
                    onClicked: exercise.toggleActivityType()
                }


//                Rectangle {
//                    height: 2 * minutesAdjustment.height + Theme.paddingMedium
//                    width: minutesAdjustment.width + Theme.paddingMedium
//                    anchors.verticalCenter: parent.verticalCenter
                    Flow {
                        id: timeAdjustment
                        flow: orientation == Orientation.Portrait ? Flow.TopToBottom : Flow.LeftToRight
                        spacing: 0.5 * Theme.paddingSmall
                        anchors.verticalCenter: parent.verticalCenter

                        ValueAdjustmentHorizontal {
                            id: minutesAdjustment
                            //anchors.horizontalCenter: parent.horizontalCenter
                            value: exercise.mins
                            maxValue: 99
                            unitLabel: "m"
                        }

                        ValueAdjustmentHorizontal {
                            id: secondsAdjustment
                            value: exercise.secs
                            maxValue: 59
                            unitLabel: "s"
                            step: 5
                        }

                    //}

                }


                //Spacer {id: fillSpace}

                IconButton {
                    anchors.verticalCenter: parent.verticalCenter
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

