import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: soundCreditsPage

    allowedOrientations: Orientation.All

    ListModel {
        id: creditsModel
        ListElement {
            files: "boxing-bell-1.wav"
            title: "Boxing Bell 1.wav"
            author: "Benboncan"
            license: "CC BY 4.0"
            url: "https://freesound.org/people/Benboncan/sounds/66952/"
        }
        ListElement {
            files: "boxing-bell-3.wav"
            title: "Boxing Bell.wav"
            author: "Benboncan"
            license: "CC BY 4.0"
            url: "https://freesound.org/people/Benboncan/sounds/66951/"
        }
        ListElement {
            files: "one.wav – ten.wav"
            title: "Simulation of NASA rocket launch 4 (countdown voice)"
            author: "Timbre"
            license: "CC BY-NC 4.0"
            url: "https://freesound.org/people/Timbre/sounds/94571/"
        }
        ListElement {
            files: "beep.wav"
            title: "01818 start beeps.wav"
            author: "Robinhood76"
            license: "CC BY-NC 4.0"
            url: "https://freesound.org/people/Robinhood76/sounds/97879/"
        }
        ListElement {
            files: "62176__robinhood76__00504-brass-fanfare-4.wav"
            title: "00504 brass fanfare 4.wav"
            author: "Robinhood76"
            license: "CC BY-NC 4.0"
            url: "https://freesound.org/people/robinhood76/sounds/62176/"
        }
        ListElement {
            files: "61234__sapht__snes-startup.wav"
            title: "SNES startup.wav"
            author: "sapht"
            license: "CC Sampling+ 1.0"
            url: "https://freesound.org/people/sapht/sounds/61234/"
        }
    }

    SilicaListView {
        anchors.fill: parent
        model: creditsModel

        header: Column {
            width: parent.width

            PageHeader {
                title: qsTr("Sound credits")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                color: Theme.secondaryColor
                text: qsTr("All sounds are from freesound.org. Files were resampled or trimmed from the originals.")
                bottomPadding: Theme.paddingMedium
            }
        }

        delegate: ListItem {
            id: creditItem
            contentHeight: creditColumn.height + Theme.paddingMedium
            onClicked: {
                if (!Qt.openUrlExternally(model.url))
                    console.warn("Failed to open URL:", model.url)
            }

            Column {
                id: creditColumn
                x: Theme.horizontalPageMargin
                y: Theme.paddingSmall
                width: parent.width - 2 * Theme.horizontalPageMargin
                spacing: Theme.paddingSmall / 2

                Label {
                    width: parent.width
                    text: model.files
                    font.pixelSize: Theme.fontSizeSmall
                    color: creditItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                    wrapMode: Text.WrapAnywhere
                }

                Label {
                    width: parent.width
                    text: model.title
                    font.pixelSize: Theme.fontSizeSmall
                    color: creditItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    wrapMode: Text.WordWrap
                }

                Label {
                    width: parent.width
                    text: model.author + " · " + model.license
                    font.pixelSize: Theme.fontSizeSmall
                    color: creditItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                }

                Label {
                    width: parent.width
                    text: model.url
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.primaryColor
                    font.underline: true
                    wrapMode: Text.WrapAnywhere
                }
            }
        }

        VerticalScrollDecorator {}
    }
}
