#include "soundplayer.h"

// Test-only stand-in for src/soundplayer.cpp, which depends on
// SailfishApp::pathTo() (sailfishapp.h) to locate sound files and is not
// usable outside the Sailfish app runtime. All tests construct ExerciseTimer
// with enableSound=false, so these methods are never invoked at runtime --
// they only need to satisfy the linker.

SoundPlayer::SoundPlayer(QObject *parent) : QObject(parent)
{
}

void SoundPlayer::playCountDownSound(int)
{
}

void SoundPlayer::playSound(SoundPlayer::Sound)
{
}

void SoundPlayer::setupSounds()
{
}
