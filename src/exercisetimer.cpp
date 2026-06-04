#include <QObjectList>
#include <QTimerEvent>
#include <QTimer>
#include <QSettings>

#include "exercisetimer.h"
#include "timedexercise.h"
#include "exerciselistmodel.h"
#include "eoqttrace.h"
#include "soundplayer.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
ExerciseTimer::ExerciseTimer(QObject *parent) :
    QObject(parent), mTotalDuration(QTime(0, 0, 0)),
    mCurrentExerciseDuration(QTime(0, 0, 0)), mCurrentExerciseIndex(0),
    mTotalRunningTime(QTime(0, 0, 0)), mCurrentRunningTime(QTime(0, 0, 0)),
    mStartTime(QTime(0, 0, 0)), mTimerInterval(100), mTimerID(0),
    mStartDelay(5), mRunning(false), mWaitingToStart(false),
    mNotifyReps(false),
    mRepSeparationMilliSecs(0), mCheckRepTimer(false),
    mEndNotificationTime(3),
    mSendEndNotification(false), mCountdownTracker(0),
    mCurrentRepNumber(0), mCurrentProgress(0), mTotalProgress(0)
{
    QSettings settings;
    if (settings.contains("start delay"))
    {
        mStartDelay = settings.value("start delay").toInt();
    }
    if (settings.contains("end notification time"))
    {
        mEndNotificationTime = settings.value("end notification time").toInt();
    }
    mModel = new ExerciseListModel(this);
    connect(mModel, &ExerciseListModel::totalDurationChanged,
            this, &ExerciseTimer::onTotalDurationChanged);
    connect(this, SIGNAL(currentExerciseFinished()), this,
            SLOT(onCurrentExerciseFinished()));
    connect(this, SIGNAL(currentRunningTimeChanged(QTime)),
            this, SLOT(onCurrentRunningTimeChanged(QTime)));
    connect(this, SIGNAL(totalRunningTimeChanged(QTime)),
            this, SLOT(onTotalRunningTimeChanged(QTime)));
    connect(this, SIGNAL(allExercisesFinished()), this,
            SLOT(onAllExercisesFinished()));
    connect(this, SIGNAL(runningStatusChanged(bool)),
            this, SLOT(onRunningStatusChanged(bool)));
    connect(this, SIGNAL(notifyRep(int)), this, SLOT(doRepetition(int)));
    connect(this, SIGNAL(currentExerciseCloseToEnd()), this,
            SLOT(onCurrentExerciseCloseToEnd()));
    mPlayer = new SoundPlayer(this);
    mPlayer->setVolume(100);
    mCountdownTimer = new QTimer(this);
    connect(mCountdownTimer, SIGNAL(timeout()), this,
            SLOT(onCountDown()));
    connect(this, SIGNAL(countDown(int)),
            mPlayer, SLOT(playCountDownSound(int)));
    //mScreenSaver = new QSystemScreenSaver(this);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::addExercise(TimedExercise *exercise, int pos)
{
    if (-1 == pos)
    {
        mModel->appendExercise(exercise);
    }
    else mModel->insertExercise(exercise, pos);

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::addRest(int mins, int secs)
{
    FUTR();
    TimedExercise* ex = new TimedExercise("rest", mins, secs, 0, this);
    mModel->appendExercise(ex);
    connect(ex, SIGNAL(durationChanged()),
            this, SLOT(calculateTotalDuration()));

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::removeExercise(int index)
{
    Q_UNUSED(index);
    // not implemented, because the model handles it nicely with QML ui.
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::start()
{
    FUTR();
    if (mRunning)
    {
        return;
    }
    if (mModel->isEmpty())
    {
        return;
    }
    if (mStartDelay > 0)
    {
        // There's a delay before really starting.
        mWaitingToStart = true;
        QTimer::singleShot(mStartDelay*1000, this,
                           SLOT(startAfterDelay()));
        startCountDown(mStartDelay);
        //mPlayer->playCountDownSound(mStartDelay);
//        QTimer countdownTimer;
//        countdownTimer.


    }
    else
    {
        // start directly
        playCurrentExercise();
    }

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::pause()
{
    if (!mRunning)
    {
        return;
    }
    killTimer(mTimerID);
    mTimerID = 0; // reset to zero
    int mSecsSinceLastTimerEvent = mStartTime.elapsed();

    mCurrentRunningTime = mCurrentRunningTime.addMSecs(mSecsSinceLastTimerEvent);
    mTotalRunningTime = mTotalRunningTime.addMSecs(mSecsSinceLastTimerEvent);
    emit totalRunningTimeChanged(mTotalRunningTime);
    emit currentRunningTimeChanged(mCurrentRunningTime);
    mRunning = false;
    emit runningStatusChanged(mRunning);

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::reset()
{
    pause();
    mCurrentRunningTime.setHMS(0, 0, 0);
    mTotalRunningTime.setHMS(0, 0, 0);
    emit currentRunningTimeChanged(mCurrentRunningTime);
    emit totalRunningTimeChanged(mTotalRunningTime);
    mCurrentExerciseIndex = 0;
    emit currentActivityChanged(getExercise(mCurrentExerciseIndex));
    mCurrentRepNumber = 0;
    emit notifyRep(mCurrentRepNumber);

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::appendDefaultExercise()
{
    FUTR();
    TimedExercise* ex = new TimedExercise();
    ex->setParent(this);
    mModel->appendExercise(ex);
    connect(ex, SIGNAL(durationChanged()),
            this, SLOT(calculateTotalDuration()));
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
ExerciseListModel* ExerciseTimer::exerciseListModel()
{
    return mModel;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::modifyExercise(int index)
{
    FUTR();
    TRACE1("modification of exercise %1 requested", index);
    emit requestModificationOfExercise(getExercise(index));

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::onCountChanged()
{
    FUTR();
    TRACE1("Count now: %1", mModel->count());
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//

void ExerciseTimer::onTotalDurationChanged(int totalDurationChangeSeconds)
{
    mTotalDuration = mTotalDuration.addSecs(totalDurationChangeSeconds);
    emit totalDurationChanged(mTotalDuration);
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
QTime ExerciseTimer::totalDuration() const
{
    return mTotalDuration;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != mTimerID)
    {
        // some other timer, is it even possible to get their events?
        return;
    }

    // The mTotalTime and mCurrentLapTime are incremented
    // at every timer event. In practise the events do not occur exactly at
    // the set interval (default 1000 ms). Therefore, we can't increment the
    // time counters by the fixed interval, but need to measure the really
    // elapsed time.

    // The time (in milliseconds) elapsed since last timer event
    int elapsedMsecs = mStartTime.elapsed();

    mStartTime.restart();
    mCurrentRunningTime = mCurrentRunningTime.addMSecs(elapsedMsecs);
    mTotalRunningTime = mTotalRunningTime.addMSecs(elapsedMsecs);

    // Check if any of the set time intervals is full and act accordingly
    if (mNotifyReps)
    {
        static int mSecsSinceLastRep(0);
        static int repsForCurrentExercise(0);
        // mSecsSinceLastRep can be a longer time than the current
        // running time only when a new exercise has started and
        // mSecsSinceLastRep holds some value from a previous
        // exercise. In that case, it should be reset to 0.
        if (mCheckRepTimer && mSecsSinceLastRep >
                mCurrentRunningTime.msecsTo(QTime(0, 0)))
        {
            mSecsSinceLastRep = 0;
            // reset also the rep counter repsForCurrentExercise(0);
            repsForCurrentExercise = 0;
            // By setting this to false we have to do this comparison
            // only once per exercise (that is not the first one to run).
            mCheckRepTimer = false;
        }

        mSecsSinceLastRep += elapsedMsecs;
        if (mSecsSinceLastRep >= mRepSeparationMilliSecs)
        {
            // increase the rep count
            repsForCurrentExercise += 1;
            emit notifyRep(repsForCurrentExercise);
            // reset the counter. It must be reset also when the
            // current activity stops and the next starts. The mCheckRepTimer
            // variable onCurrentExerciseFinished() take care of that.
            mSecsSinceLastRep = 0;
        }
    }

    // Check if the full seconds have changed
    static int previousSec = 0;
    int currentSec = mCurrentRunningTime.second();
    if (currentSec !=  previousSec)
    {
        // The seconds part of the running time has changed.
        // Send time change signals.
        emit currentRunningTimeChanged(mCurrentRunningTime);
        emit totalRunningTimeChanged(mTotalRunningTime);
//        mTotalRunningTime = mTotalRunningTime +
//        emit secondsChanged(mTotalRunningTime);
        if (mSendEndNotification)
        {
            // send notification before the end of current activity
            if (mCurrentRunningTime >= mCurrentExerciseEndNotificationTime)
            {
                emit currentExerciseCloseToEnd();
            }
        }
        if (mCurrentRunningTime >= mCurrentExerciseDuration)
        {
            emit currentExerciseFinished();
            TRACE("Exercise reached its duration.");
        }
    }
    previousSec = currentSec;


}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
QTime ExerciseTimer::currentRunningTime() const
{
    return mCurrentRunningTime;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
bool ExerciseTimer::running() const
{
    return mRunning;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::playCurrentExercise()
{
    FUTR();
    if (mRunning || mWaitingToStart)
    {
        return;
    }

    TimedExercise* ex = getExercise(mCurrentExerciseIndex);
    emit currentActivityChanged(ex);
    if (ex->activityType() == "work")
    {
        // Play the round start sound only for work periods, not for rest
        mPlayer->playSound(SoundPlayer::RoundStartSound);
    }
    mCurrentExerciseDuration = QTime(0, ex->mins(), ex->secs());
    if (ex->activityType() == "work" && ex->rpm() != 0)
    {
        mNotifyReps = true;
        mRepSeparationMilliSecs = (int) 1000 * ex->repSeparation(); //  in secs
    }
    else
    {
        mNotifyReps = false;
    }
    if (mEndNotificationTime > 0)
    {
        // A notification will be sent before the end of the current
        // activity

        // Check that the current period is longer than the pre-warning
        // time, which may be up to 60 s.
        if (mCurrentExerciseDuration.minute() >= 1 ||
                mCurrentExerciseDuration.second() > mEndNotificationTime)
        {
            mCurrentExerciseEndNotificationTime =
                mCurrentExerciseDuration.addSecs(-mEndNotificationTime);
            mSendEndNotification = true;
        }
    }
    mStartTime = QTime::currentTime();
    mTimerID = startTimer(mTimerInterval);
    if (!mTimerID)
    {
        // The timer could not be started
        return;
    }
    // Timer events start coming now and are handled in timerEvent()
    mRunning = true;
    emit runningStatusChanged(true);


}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
TimedExercise * ExerciseTimer::getExercise(int index)
{
    return (TimedExercise*)mModel->at(index);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::onCurrentExerciseFinished()
{
    FUTR();
    if (mRunning)
    {
        // Stops the timer for current exercise
        killTimer(mTimerID);
        mTimerID = 0; // reset to zero
//        int mSecsSinceLastTimerEvent = mStartTime.elapsed();
//        mCurrentRunningTime = mCurrentRunningTime.
//                addMSecs(mSecsSinceLastTimerEvent);

        mCurrentRunningTime = QTime(0, 0, 0);
        emit currentRunningTimeChanged(mCurrentRunningTime);
        mRunning = false;
        // The static int variable mSecsSinceLastRep in timerEvent function
        // may have some leftover value. This bool forces it to be reset.
        mCheckRepTimer = true;
        emit runningStatusChanged(mRunning);
        // reset rep counter
        mCurrentRepNumber = 0;
        emit notifyRep(mCurrentRepNumber);
        // Check if there is a next activity to perform
        if (mModel->size() > mCurrentExerciseIndex + 1)
        {
            // There's an exercise
            mCurrentExerciseIndex += 1;
            // Check if the next activity is work or rest. If it is work,
            // do not play round end sound for the just ended round. Instead
            // the round start sound will be played. If the next round is rest,
            // play the round end sound.
            if (getExercise(mCurrentExerciseIndex)->activityType() == "rest")
            {
                mPlayer->playSound(SoundPlayer::RoundEndSound);
            }
            TRACE1("Switching to activity %1", mCurrentExerciseIndex + 1);
            playCurrentExercise();

        }
        else
        {
            emit allExercisesFinished();
            mTotalRunningTime.setHMS(0, 0, 0);
            emit totalRunningTimeChanged(mTotalRunningTime);

        }


    }

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::onCurrentRunningTimeChanged(QTime runTime)
{
    mCurrentProgress = ((double)runTime.msecsTo(QTime(0, 0, 0)) /
                        mCurrentExerciseDuration.msecsTo(QTime(0, 0, 0)));
    emit currentProgressChanged(mCurrentProgress);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::onAllExercisesFinished()
{
    FUTR();
    mPlayer->playSound(SoundPlayer::AllDoneSound);
    mCurrentExerciseIndex = 0;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::onRunningStatusChanged(bool running)
{
    FUTR();
    if (running)
    {
        //play start sound etc -- done elsewhere
        //mScreenSaver->setScreenSaverInhibited(true);
    }
    else
    {
        // play stop sound
        //mScreenSaver->setScreenSaverInhibited(false);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::doRepetition(int repNumber)
{
    FUTR();
    mPlayer->playSound(SoundPlayer::RepSound);
    FUNC_TRACE(QString("Time now: %1")
               .arg(mCurrentRunningTime.toString("mm:ss.zz")));
    TRACE1("Did rep number %1", repNumber);
    mCurrentRepNumber = repNumber;

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::setStartDelay(int seconds)
{
    FUTR();
    if (seconds != mStartDelay)
    {
        if (seconds >= 0 && seconds < 60)
        {
             mStartDelay = seconds;
             emit startDelayChanged(mStartDelay);
             QSettings settings;
             settings.setValue("start delay", mStartDelay);
        }

    }

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
int ExerciseTimer::startDelay() const
{
    return mStartDelay;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::startAfterDelay()
{
    mWaitingToStart = false;
    playCurrentExercise();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::onCurrentExerciseCloseToEnd()
{
    FUTR();
    startCountDown(mEndNotificationTime);
    mSendEndNotification = false;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
QTime ExerciseTimer::totalRunningTime() const
{
    return mTotalRunningTime;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::startCountDown(int durationSeconds)
{
    mCountdownTracker = durationSeconds;
    // must invoke the onCountDown slot immediately. Otherwise there's a
    // one-second delay.
    onCountDown();
    mCountdownTimer->start(1000);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::onCountDown()
{
    if (mCountdownTracker > 0)
    {
        emit countDown(mCountdownTracker);
        mCountdownTracker -= 1;
    }
    else // tracker == 0
    {
        mCountdownTimer->stop();
        // it's useful to send also the zero countdown signal, because
        // ui knows it can hide the countdown number.
        emit countDown(0);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
int ExerciseTimer::endWarningTime() const
{
    return mEndNotificationTime;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::setEndWarningTime(int seconds)
{
    FUTR();
    if (seconds != mEndNotificationTime)
    {
        if (seconds >= 0 && seconds < 60)
        {
             mEndNotificationTime = seconds;
             emit endWarningTimeChanged(mEndNotificationTime);
             QSettings settings;
             settings.setValue("end notification time", mEndNotificationTime);
        }

    }

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
TimedExercise * ExerciseTimer::currentActivity()
{
    if (mModel->size() == 0)
    {
        return 0;
    }
    else
    {
        return getExercise(mCurrentExerciseIndex);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
int ExerciseTimer::currentRepNumber() const
{
    return mCurrentRepNumber;
}

double ExerciseTimer::currentProgress() const
{
    return mCurrentProgress;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
double ExerciseTimer::totalProgress() const
{
    return mTotalProgress;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::onTotalRunningTimeChanged(QTime totalRunTime)
{
    mTotalProgress = ((double)totalRunTime.msecsTo(QTime(0, 0, 0)) /
                        mTotalDuration.msecsTo(QTime(0, 0, 0)));
    emit totalProgressChanged(mTotalProgress);
}
