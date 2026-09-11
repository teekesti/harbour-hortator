import QtQuick 2.0
import Sailfish.Silica 1.0
import io.github.teekesti.exercisetimer 1.0
import "."

Column {
    id: setCard

    property var exerciseSet: null
    property int setIndex
    property bool highlighted

    // First-use hints (#14, ADR-0019): activateAddHint points out this
    // card's own add-exercise button; activateExerciseContextMenuHint is
    // forwarded to the first exercise row. Driven by the page
    // orchestrating the hint sequence; this card just reports back.
    property bool activateAddHint: false
    property bool activateExerciseContextMenuHint: false
    signal addHintFinished()
    signal contextMenuHintFinished()
    signal hintInteracted()

    Rectangle {
        id: card
        width: parent.width - 2 * Theme.horizontalPageMargin
        anchors.horizontalCenter: parent.horizontalCenter
        height: cardColumn.height + Theme.paddingMedium
        radius: Theme.paddingMedium

        border.width: 1
        border.color: (!exerciseSet || !exerciseSet.isValid)
                      ? Theme.errorColor
                      : Theme.rgba(Theme.highlightBackgroundColor, 0.5)

        color: highlighted
               ? Theme.rgba(Theme.highlightColor, Theme.opacityLow)
               : Theme.rgba(Theme.highlightBackgroundColor, 0.1)

        Column {
            id: cardColumn
            width: parent.width
            anchors.top: parent.top
            anchors.topMargin: Theme.paddingSmall

            Row {
                id: setHeaderRow
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                height: implicitHeight + Theme.paddingSmall
                spacing: Theme.paddingMedium

                Label {
                    text: qsTr("Set %1").arg(setIndex + 1)
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                }

                RoundCountAdjustment {
                    id: setRoundsAdjustment
                    value: exerciseSet ? exerciseSet.rounds : 1
                    minValue: 1
                    maxValue: 99
                }
                Binding { target: exerciseSet; property: "rounds"; value: setRoundsAdjustment.value }

                IconButton {
                    anchors.verticalCenter: parent.verticalCenter
                    icon.source: "image://theme/icon-m-add?" + (pressed
                              ? Theme.highlightColor
                              : Theme.primaryColor)

                    property bool longPressed: false

                    onPressed: longPressed = false
                    onPressAndHold: {
                        longPressed = true
                        setCard.hintInteracted()
                        pageStack.push(Qt.resolvedUrl("../PickExerciseTemplateDialog.qml"),
                                       {setIndex: setIndex})
                    }
                    onClicked: {
                        if (!longPressed) {
                            exerciseTimer.addDefaultExerciseToSet(setIndex)
                            setCard.hintInteracted()
                        }
                    }

                    FirstUseHint {
                        anchors.centerIn: parent
                        active: activateAddHint
                        onFinished: setCard.addHintFinished()
                    }
                }
            }

            Rectangle {
                width: parent.width - 2 * Theme.horizontalPageMargin
                anchors.horizontalCenter: parent.horizontalCenter
                height: 1
                color: Theme.rgba(Theme.highlightBackgroundColor, 0.4)
                visible: exerciseSet && exerciseSet.count > 0
            }

            Repeater {
                id: exerciseRepeater
                model: exerciseSet ? exerciseSet.count : 0

                delegate: ExerciseRow {
                    width: card.width
                    setIndex: setCard.setIndex
                    exerciseIndex: index
                    exercise: exerciseSet ? exerciseSet.at(index) : null
                    activateContextMenuHint: setCard.activateExerciseContextMenuHint && index === 0
                    onContextMenuHintFinished: setCard.contextMenuHintFinished()
                    onHintInteracted: setCard.hintInteracted()
                }
            }
        }
    }
}
