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
                text: "harbour-exercisetimer"
                font.pixelSize: Theme.fontSizeLarge
                color: Theme.highlightColor
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                //: Version label on the About page, %1 is the version number
                text: qsTr("Version %1").arg(appVersion)
                color: Theme.secondaryColor
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                text: qsTr("An interval timer for timed work and rest sets. Build a workout as a list of exercises, then run it with a countdown, progress bars, and audio cues for reps and round transitions.")
            }

            SectionHeader {
                text: qsTr("Usage")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                text: qsTr("Add exercises using the \"+\" button, set work and rest durations and the number of repetitions, then tap the play button to start the workout.")
            }

            SectionHeader {
                text: qsTr("Source code")
            }

            BackgroundItem {
                width: parent.width
                height: sourceLabel.height + 2 * Theme.paddingSmall
                onClicked: {
                    if (!Qt.openUrlExternally("https://github.com/teekesti/harbour-exercisetimer"))
                        console.warn("Failed to open source URL")
                }

                Label {
                    id: sourceLabel
                    x: Theme.horizontalPageMargin
                    y: Theme.paddingSmall
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    wrapMode: Text.WrapAnywhere
                    text: "https://github.com/teekesti/harbour-exercisetimer"
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
