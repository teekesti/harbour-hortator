import QtQuick 2.0
import Sailfish.Silica 1.0

MouseArea {
    id: root

    // Julkiset ominaisuudet
    property url iconSource: ""
    property int baseInterval: 300   // Aloitusnopeus (millisekuntia per klikkaus)
    property int minInterval: 50     // Maksiminopeus (kuinka nopeaksi toisto voi mennä)
    property int stepDecrease: 40    // Kuinka paljon interval lyhenee per askel
    property int delay: 450          // Viive ennen kuin automaattinen toisto alkaa

    signal triggered()

    width: Theme.itemSizeSmall
    height: Theme.itemSizeSmall
    opacity: pressed ? 0.6 : 1.0

    // SailfishOS:n oma Icon-tyyppi hoitaa värityksen automaattisesti teeman mukaan
    Icon {
        anchors.centerIn: parent
        source: root.iconSource
        width: Theme.iconSizeMedium
        height: Theme.iconSizeMedium
    }

    // Toistoajastin dynaamisella intervallilla
    Timer {
        id: repeatTimer
        repeat: true
        onTriggered: {
            root.triggered()
            // Nopeutetaan toistoa pienentämällä intervallia askel kerrallaan
            if (interval > root.minInterval) {
                interval = Math.max(root.minInterval, interval - root.stepDecrease)
            }
        }
    }

    // Viiveajastin ennen toiston alkamista
    Timer {
        id: initialDelayTimer
        interval: root.delay
        repeat: false
        onTriggered: {
            repeatTimer.interval = root.baseInterval // Nollataan aloitusnopeus
            repeatTimer.start()
        }
    }

    onPressed: {
        root.triggered() // Ensimmäinen klikkaus heti painettaessa
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
