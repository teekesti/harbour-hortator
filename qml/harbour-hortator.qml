import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"
import io.github.teekesti.exercisetimer 1.0

ApplicationWindow {
    initialPage: Component { FirstPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
    background.image: Qt.resolvedUrl("images/kettlebell_stopwatch_with_background.png")
    background.filter: "none"

    function shouldShowRunPage() {
        return (exerciseTimer.running || exerciseTimer.waitingToStart)
                && pageStack.currentPage.objectName !== "runPage"
    }

    Connections {
        target: Qt.application
        onActiveChanged: {
            if (Qt.application.active && shouldShowRunPage()) {
                pageStack.push(Qt.resolvedUrl("pages/RunPage.qml"))
            }
        }
    }

    // Cover play fires onActiveChanged before exerciseTimer.start() runs,
    // so also respond to timer state changes while already in foreground.
    Connections {
        target: exerciseTimer
        onRunningStatusChanged: {
            if (Qt.application.active && shouldShowRunPage()) {
                pageStack.push(Qt.resolvedUrl("pages/RunPage.qml"))
            }
        }
        onWaitingToStartChanged: {
            if (Qt.application.active && shouldShowRunPage()) {
                pageStack.push(Qt.resolvedUrl("pages/RunPage.qml"))
            }
        }
    }
}
