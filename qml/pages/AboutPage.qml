 import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: aboutPage

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height + Theme.paddingLarge

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("About")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                text: applicationDisplayName
                font.pixelSize: Theme.fontSizeLarge
                color: Theme.highlightColor
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                text: qsTr("Version %1").arg(appVersion)
                color: Theme.secondaryColor
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                text: qsTr("A versatile interval timer for workouts. Easily follow your workout with a countdown, progress bars, and audio cues for repetitions and round transitions.")
            }

            SectionHeader {
                text: qsTr("Usage")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                text: qsTr("Add sets with the \"+\" button at the bottom of the screen. Within a set, add exercises with the \"+\" button. " +
                           "A single tap adds a default exercise, and a long press lets you load a saved one from your exercise library. " +
                           "Set each exercise as work or rest and adjust its duration. " +
                           "The context menu also lets you set the number of repetitions for each exercise. " +
                           "Set the number of rounds for whole sets or individual exercises. " +
                           "Tap the play button to start the workout.")
            }

            SectionHeader {
                text: qsTr("Source code")
            }

            BackgroundItem {
                width: parent.width
                height: sourceLabel.height + 2 * Theme.paddingSmall
                onClicked: {
                    if (!Qt.openUrlExternally("https://github.com/teekesti/harbour-hortator"))
                        console.warn("Failed to open source URL")
                }

                Label {
                    id: sourceLabel
                    x: Theme.horizontalPageMargin
                    y: Theme.paddingSmall
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    wrapMode: Text.WrapAnywhere
                    text: "https://github.com/teekesti/harbour-hortator"
                    color: Theme.primaryColor
                    font.underline: true
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Sound credits")
                onClicked: pageStack.push(Qt.resolvedUrl("SoundCreditsPage.qml"))
            }
        }

        VerticalScrollDecorator {}
    }
}
