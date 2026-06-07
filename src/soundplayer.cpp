#include <QDir>
#include <QMap>
#include <sailfishapp.h>
#include "soundplayer.h"
#include "eoqttrace.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
SoundPlayer::SoundPlayer(QObject *parent) :
    QObject(parent)
{
    setupSounds();


}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void SoundPlayer::playCountDownSound(int number)
{
    if (mCountdownSounds.contains(number))
    {
        mCountdownSounds[number]->play();
    }
    else if (0 == number)
    {
        return; // do not play default sound for zero
    }
    else
    {
        mDefaultSound.play();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void SoundPlayer::setupSounds()
{
    // Countdown sounds
    QDir soundDir = QDir(SailfishApp::pathTo("sounds").toLocalFile());
    for (int i =  0; i < 11; i++)
    {
        mCountdownSounds[i] = new QSoundEffect(this);
    }
    mCountdownSounds[1]->setSource(QUrl::fromLocalFile(soundDir.filePath("one.wav")));
    mCountdownSounds[2]->setSource(QUrl::fromLocalFile(soundDir.filePath("two.wav")));
    mCountdownSounds[3]->setSource(QUrl::fromLocalFile(soundDir.filePath("three.wav")));
    mCountdownSounds[4]->setSource(QUrl::fromLocalFile(soundDir.filePath("four.wav")));
    mCountdownSounds[5]->setSource(QUrl::fromLocalFile(soundDir.filePath("five.wav")));
    mCountdownSounds[6]->setSource(QUrl::fromLocalFile(soundDir.filePath("six.wav")));
    mCountdownSounds[7]->setSource(QUrl::fromLocalFile(soundDir.filePath("seven.wav")));
    mCountdownSounds[8]->setSource(QUrl::fromLocalFile(soundDir.filePath("eight.wav")));
    mCountdownSounds[9]->setSource(QUrl::fromLocalFile(soundDir.filePath("nine.wav")));
    mCountdownSounds[10]->setSource(QUrl::fromLocalFile(soundDir.filePath("ten.wav")));
    mDefaultSound.setSource(QUrl::fromLocalFile(soundDir.filePath("beep.wav")));
    mRoundStartSound.setSource(QUrl::fromLocalFile(soundDir.filePath("boxing-bell-1.wav")));
    mRoundEndSound.setSource(QUrl::fromLocalFile(soundDir.filePath("boxing-bell-3.wav")));
    mRepSound.setSource(QUrl::fromLocalFile(soundDir.filePath("61234__sapht__snes-startup.wav")));
    mAllDoneSound.setSource(QUrl::fromLocalFile(soundDir.filePath("62176__robinhood76__00504-brass-fanfare-4.wav")));

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void SoundPlayer::playSound(SoundPlayer::Sound sound)
{
    switch (sound)
    {
    case RoundStartSound:
        mRoundStartSound.play();
        break;
    case RoundEndSound:
        mRoundEndSound.play();
        break;
    case RepSound:
        mRepSound.play();
        break;
    case AllDoneSound:
        mAllDoneSound.play();
    }
}
