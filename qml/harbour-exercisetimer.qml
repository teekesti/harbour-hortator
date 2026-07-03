import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"
import com.appiukko.exercisetimer 1.0

ApplicationWindow {
    initialPage: Component { FirstPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations

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
