import QtQuick 2.0
import Sailfish.Silica 1.0
import com.appiukko.exercisetimer 1.0
import "."

Column {
    id: setCard

    property var exerciseSet: null
    property int setIndex
    property bool highlighted

    spacing: Theme.paddingSmall

    Rectangle {
        id: setHeaderBackground
        width: parent.width - 2 * Theme.horizontalPageMargin
        anchors.horizontalCenter: parent.horizontalCenter
        height: setHeaderRow.height + Theme.paddingMedium
        radius: Theme.paddingSmall

        color: {
            if (!exerciseSet || !exerciseSet.isValid) {
                return Theme.errorColor
            }
            if (highlighted) {
                return Theme.rgba(Theme.highlightColor, Theme.opacityLow)
            }
            return Theme.rgba(Theme.highlightBackgroundColor, 0.4)
        }

        Row {
            id: setHeaderRow
            anchors {
                left: parent.left; right: parent.right
                verticalCenter: parent.verticalCenter
                leftMargin: Theme.horizontalPageMargin
                rightMargin: Theme.horizontalPageMargin
            }
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
                    pageStack.push(Qt.resolvedUrl("../PickExerciseTemplateDialog.qml"),
                                   {setIndex: setIndex})
                }
                onClicked: if (!longPressed) exerciseTimer.addDefaultExerciseToSet(setIndex)
            }
        }
    }

    Repeater {
        id: exerciseRepeater
        model: exerciseSet ? exerciseSet.count : 0

        delegate: ExerciseRow {
            width: setCard.width
            setIndex: setCard.setIndex
            exerciseIndex: index
            exercise: exerciseSet ? exerciseSet.at(index) : null
        }
    }
}
