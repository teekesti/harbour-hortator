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

    Connections {
        target: Qt.application
        onActiveChanged: {
            if (Qt.application.active && exerciseTimer.running
                    && pageStack.currentPage.objectName !== "runPage") {
                pageStack.push(Qt.resolvedUrl("pages/RunPage.qml"))
            }
        }
    }
}
