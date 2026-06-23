import QtQuick 2.0
import Sailfish.Silica 1.0

MouseArea {
    id: root

    property url iconSource: ""
    property int baseInterval: 300   // starting speed (ms per repeat)
    property int minInterval: 50     // fastest the repeat can accelerate to
    property int stepDecrease: 40    // how much interval shrinks per step
    property int delay: 450          // delay before auto-repeat starts

    signal triggered()

    width: Theme.itemSizeSmall
    height: Theme.itemSizeSmall
    opacity: pressed ? 0.6 : 1.0

    Icon {
        anchors.centerIn: parent
        source: root.iconSource
        width: Theme.iconSizeMedium
        height: Theme.iconSizeMedium
    }

    Timer {
        id: repeatTimer
        repeat: true
        onTriggered: {
            root.triggered()
            if (interval > root.minInterval) {
                interval = Math.max(root.minInterval, interval - root.stepDecrease)
            }
        }
    }

    Timer {
        id: initialDelayTimer
        interval: root.delay
        repeat: false
        onTriggered: {
            repeatTimer.interval = root.baseInterval
            repeatTimer.start()
        }
    }

    onPressed: {
        root.triggered()
        initialDelayTimer.start()
    }

    onReleased: {
        initialDelayTimer.stop()
        repeatTimer.stop()
    }

    onCanceled: {
        initialDelayTimer.stop()
        repeatTimer.stop()
    }
}
