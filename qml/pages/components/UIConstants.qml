pragma Singleton
import QtQuick 2.0
import Sailfish.Silica 1.0

QtObject {
    // string constants for strings that are used in more than one place
    readonly property string currentActivityLabel: qsTr("Current exercise")
    readonly property string totalProgressLabel: qsTr("Total progress")

    // colour definitions
    readonly property color workColor: Theme.rgba(Theme.highlightBackgroundColor, 0.25)
    readonly property color restColor: Theme.rgba(Theme.highlightDimmerColor, 0.10)


}
