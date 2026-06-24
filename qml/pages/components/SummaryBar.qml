import QtQuick 2.0
import Sailfish.Silica 1.0

// A horizontal bar of work/rest-colored sub-rectangles, one per Exercise
// occurrence, sized proportionally to that occurrence's duration. Used in
// both the editor (the live Draft) and the History list (a saved
// snapshot) to give an at-a-glance shape of a workout. See ADR-0013.
Item {
    id: root

    // Array of {activityType, durationSeconds} objects, in play order.
    property var playSequence: []
    property int barHeight: Theme.paddingSmall

    height: barHeight

    readonly property int totalDurationSeconds: {
        var total = 0
        for (var i = 0; i < playSequence.length; i++) {
            total += playSequence[i].durationSeconds
        }
        return total
    }

    Rectangle {
        anchors.fill: parent
        radius: height / 2
        color: Theme.rgba(Theme.highlightDimmerColor, 0.10)
        visible: root.totalDurationSeconds === 0
    }

    Row {
        anchors.fill: parent
        visible: root.totalDurationSeconds > 0

        Repeater {
            model: root.playSequence

            Rectangle {
                height: root.barHeight
                width: root.totalDurationSeconds > 0
                       ? root.width * (modelData.durationSeconds / root.totalDurationSeconds)
                       : 0
                color: modelData.activityType === "work"
                       ? Theme.rgba(Theme.highlightBackgroundColor, 0.25)
                       : Theme.rgba(Theme.highlightDimmerColor, 0.10)
            }
        }
    }
}
