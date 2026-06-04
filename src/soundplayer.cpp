#include <QDir>
#include <QMap>
#include "soundplayer.h"
#include "eoqttrace.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
SoundPlayer::SoundPlayer(QObject *parent) :
    QMediaPlayer(parent)
{
    connect(this, SIGNAL(error(QMediaPlayer::Error)), this,
            SLOT(errorOccurred(QMediaPlayer::Error)));
    setupSounds();


}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void SoundPlayer::errorOccurred(QMediaPlayer::Error error)
{
    Q_UNUSED(error)
    TRACE(errorString());
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void SoundPlayer::playCountDownSound(int number)
{
    if (mCountdownSounds.contains(number))
    {
        setMedia(mCountdownSounds.value(number));
    }
    else if (0 == number)
    {
        return; // do not play default sound for zero
    }
    else
    {
        setMedia(mDefaultSound);
    }
    play();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void SoundPlayer::setupSounds()
{
    // Countdown sounds
    QDir soundDir = QDir("sounds");

    mCountdownSounds[1] = QMediaContent(QUrl::fromLocalFile(soundDir.filePath("one.wav")));
    mCountdownSounds[2] = QMediaContent(QUrl::fromLocalFile(soundDir.filePath("two.wav")));
    mCountdownSounds[3] = QMediaContent(QUrl::fromLocalFile(soundDir.filePath("three.wav")));
    mCountdownSounds[4] =  QMediaContent(QUrl::fromLocalFile(soundDir.filePath("four.wav")));
    mCountdownSounds[5] =  QMediaContent(QUrl::fromLocalFile(soundDir.filePath("five.wav")));
    mCountdownSounds[6] =  QMediaContent(QUrl::fromLocalFile(soundDir.filePath("six.wav")));
    mCountdownSounds[7] =  QMediaContent(QUrl::fromLocalFile(soundDir.filePath("seven.wav")));
    mCountdownSounds[8] =  QMediaContent(QUrl::fromLocalFile(soundDir.filePath("eight.wav")));
    mCountdownSounds[9] = QMediaContent(QUrl::fromLocalFile(soundDir.filePath("nine.wav")));
    mCountdownSounds[10] = QMediaContent(QUrl::fromLocalFile(soundDir.filePath("ten.wav")));
    mDefaultSound = QMediaContent(QUrl::fromLocalFile(soundDir.filePath("beep.wav")));
    mRoundStartSound = QMediaContent(QUrl::fromLocalFile(soundDir.filePath("boxing-bell-1.wav")));
    mRoundEndSound = QMediaContent(QUrl::fromLocalFile(soundDir.filePath("boxing-bell-3.wav")));
    mRepSound = QMediaContent(QUrl::fromLocalFile(soundDir.filePath("61234__sapht__snes-startup.wav")));
    mAllDoneSound = QMediaContent(QUrl::fromLocalFile(soundDir.filePath("62176__robinhood76__00504-brass-fanfare-4.wav")));

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void SoundPlayer::playSound(SoundPlayer::Sound sound)
{
    QMediaContent content;
    switch (sound)
    {
    case RoundStartSound:
        content = mRoundStartSound;
        break;
    case RoundEndSound:
        content = mRoundEndSound;
        break;
    case RepSound:
        content = mRepSound;
        break;
    case AllDoneSound:
        content = mAllDoneSound;
    }
    setMedia(content);
    play();
}
