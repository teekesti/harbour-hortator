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
    // Countdown sounds. The architecture allows separate sounds for counting from 10 to 0,
    // but setting here the same bell sound for counts 3 to 1 and no sound for the others.
    QSoundEffect* emptySound = new QSoundEffect(this);
    QDir soundDir = QDir(SailfishApp::pathTo("sounds").toLocalFile());
    QSoundEffect* countdownBell = new QSoundEffect(this);
    countdownBell->setSource(QUrl::fromLocalFile(soundDir.filePath("countdown_bell_G.wav")));

    for (int i =  0; i < 11; i++)
    {
        if (i >= 1 && i<= 3)
        {
            mCountdownSounds[i] = countdownBell;
        }
        else {
            mCountdownSounds[i] = emptySound;
        }

    }
    // Also no source set for mDefaultSound, the default sound for counts > 10
    mRoundStartSound.setSource(QUrl::fromLocalFile(soundDir.filePath("bell-high_C.wav")));
    mRoundEndSound.setSource(QUrl::fromLocalFile(soundDir.filePath("bell-low_C.wav")));
    mRepSound.setSource(QUrl::fromLocalFile(soundDir.filePath("vocoder_whipbell.wav")));
    mAllDoneSound.setSource(QUrl::fromLocalFile(soundDir.filePath("big_fanfare_in_C.wav")));

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
