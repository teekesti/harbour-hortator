/*! SoundPlayer class is a subclass of QMediaPlayer with specific
functionality for playing sounds specific to sportst timer events. */

#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QMediaPlayer>
#include <QHash>

//class QMediaPlaylist;


class SoundPlayer : public QMediaPlayer
{
    Q_OBJECT
public:
    explicit SoundPlayer(QObject *parent = 0);

    enum Sound{RoundStartSound, RoundEndSound, RepSound, AllDoneSound};
signals:

public slots:
    /*! Play a countdown sound
    @param number The second (before start) for which the sound should
    be played. E.g. for three seconds before start, a sound "three.wav"
    could be played. If there is no specific sound for this number, a
    default sound will be played. */
    void playCountDownSound(int number);
    /*! Play a sound corresponding to the given situation
    @param sound, an enum describing the purpose of the
    sound. */
    void playSound(Sound sound);

private slots:
    void errorOccurred(QMediaPlayer::Error error);
    void setupSounds();

private: // data

    QHash<int, QMediaContent> mCountdownSounds;
    QMediaContent mDefaultSound;
    QMediaContent mRoundStartSound;
    QMediaContent mRoundEndSound;
    QMediaContent mRepSound;
    QMediaContent mAllDoneSound;


};

#endif // SOUNDPLAYER_H
