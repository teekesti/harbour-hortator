#include <QObjectList>
#include <QTimerEvent>
#include <QTimer>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>

#include "exercisetimer.h"
#include "timedexercise.h"
#include "exerciseset.h"
#include "exerciselistmodel.h"
#include "eoqttrace.h"
#include "soundplayer.h"
#include "workouthistory.h"
#include "exercisetemplatelibrary.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
ExerciseTimer::ExerciseTimer(QObject *parent, bool enableSound) :
    QObject(parent), mTotalDuration(QTime(0, 0, 0)),
    mCurrentExerciseDuration(QTime(0, 0, 0)), mCurrentExerciseIndex(0),
    mTotalRunningTime(QTime(0, 0, 0)), mCurrentRunningTime(QTime(0, 0, 0)),
    mStartTime(QTime(0, 0, 0)), mTimerInterval(100), mTimerID(0),
    mStartDelay(5), mRunning(false), mPaused(false), mWaitingToStart(false),
    mNotifyReps(false),
    mRepSeparationMilliSecs(0), mCheckRepTimer(false),
    mEndNotificationTime(3), mMuteSounds(false), mSkipLastRest(false),
    mSendEndNotification(false), mCountdownTracker(0),
    mCurrentRepNumber(0), mCurrentProgress(0), mTotalProgress(0),
    mAllValid(false), mShowFirstUseHints(false)
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
    if (settings.contains("mute sounds"))
    {
        mMuteSounds = settings.value("mute sounds").toBool();
    }
    if (settings.contains("skip last rest"))
    {
        mSkipLastRest = settings.value("skip last rest").toBool();
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
    mPlayer = enableSound ? new SoundPlayer(this) : nullptr;
    mCountdownTimer = new QTimer(this);
    connect(mCountdownTimer, SIGNAL(timeout()), this,
            SLOT(onCountDown()));
    connect(this, SIGNAL(countDown(int)),
            this, SLOT(onCountDownForSound(int)));
    //mScreenSaver = new QSystemScreenSaver(this);

    mHistory = new WorkoutHistory(this);
    mTemplateLibrary = new ExerciseTemplateLibrary(this);

    mDraftSaveTimer = new QTimer(this);
    mDraftSaveTimer->setSingleShot(true);
    mDraftSaveTimer->setInterval(300);
    connect(mDraftSaveTimer, &QTimer::timeout, this, &ExerciseTimer::saveDraftNow);

    QFile draftFile(draftFilePath());
    if (draftFile.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(draftFile.readAll());
        loadModelFromJson(doc.array());
    }
    markDraftSynced();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::addSet(ExerciseSet *set, int pos)
{
    if (-1 == pos)
    {
        mModel->appendSet(set);
    }
    else mModel->insertSet(set, pos);
    connect(set, &ExerciseSet::validityChanged,
            this, &ExerciseTimer::onSetValidityChanged);
    checkOverallValidity();
    rebuildPlaySequence();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::removeSet(int index)
{
    mModel->removeSet(index);
    checkOverallValidity();
    rebuildPlaySequence();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::addExerciseToSet(int setIndex, TimedExercise *exercise, int pos)
{
    if (setIndex < 0 || setIndex >= mModel->count())
    {
        return;
    }
    ExerciseSet *set = mModel->at(setIndex);
    if (-1 == pos)
    {
        set->appendExercise(exercise);
    }
    else set->insertExercise(exercise, pos);
    connect(exercise, &TimedExercise::activityTypeChanged,
            this, &ExerciseTimer::onExerciseChangedForDraft);
    connect(exercise, &TimedExercise::nameChanged,
            this, &ExerciseTimer::onExerciseChangedForDraft);
    connect(exercise, &TimedExercise::repsChanged,
            this, &ExerciseTimer::onExerciseChangedForDraft);
    connect(exercise, &TimedExercise::nameChanged,
            this, &ExerciseTimer::onExerciseChangedForTemplate);
    connect(exercise, &TimedExercise::minsChanged,
            this, &ExerciseTimer::onExerciseChangedForTemplate);
    connect(exercise, &TimedExercise::secsChanged,
            this, &ExerciseTimer::onExerciseChangedForTemplate);
    connect(exercise, &TimedExercise::repsChanged,
            this, &ExerciseTimer::onExerciseChangedForTemplate);
    checkOverallValidity();
    rebuildPlaySequence();
    emit playSequenceChanged();
    scheduleDraftSave();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::addDefaultExerciseToSet(int setIndex)
{
    FUTR();
    addExerciseToSet(setIndex, new TimedExercise(), -1);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::addRestToSet(int setIndex, int mins, int secs)
{
    FUTR();
    TimedExercise* ex = new TimedExercise("rest", mins, secs, 0);
    addExerciseToSet(setIndex, ex, -1);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::removeExerciseFromSet(int setIndex, int exerciseIndex)
{
    if (setIndex < 0 || setIndex >= mModel->count())
    {
        return;
    }
    ExerciseSet *set = mModel->at(setIndex);
    set->removeExercise(exerciseIndex);
    checkOverallValidity();
    rebuildPlaySequence();
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
    if (mPaused)
    {
        mPaused = false;
        emit pausedChanged(mPaused);
    }
    rebuildPlaySequence();
    if (mPlaySequence.isEmpty())
    {
        return;
    }
    if (mStartDelay > 0)
    {
        // There's a delay before really starting.
        mWaitingToStart = true;
        emit waitingToStartChanged(mWaitingToStart);
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
    mPaused = true;
    emit pausedChanged(mPaused);

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::reset()
{
    if (mWaitingToStart)
    {
        mWaitingToStart = false;
        emit waitingToStartChanged(mWaitingToStart);
    }
    pause();
    rebuildPlaySequence();
    mCurrentRunningTime.setHMS(0, 0, 0);
    mTotalRunningTime.setHMS(0, 0, 0);
    emit currentRunningTimeChanged(mCurrentRunningTime);
    emit totalRunningTimeChanged(mTotalRunningTime);
    mCurrentExerciseIndex = 0;
    emit currentActivityChanged(getExercise(mCurrentExerciseIndex));
    emit currentDurationChanged(currentDuration());
    mCurrentRepNumber = 0;
    emit notifyRep(mCurrentRepNumber);
    mPaused = false;
    emit pausedChanged(mPaused);

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::appendDefaultSet()
{
    FUTR();
    ExerciseSet* set = new ExerciseSet();
    set->appendExercise(new TimedExercise());
    addSet(set, -1);
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
void ExerciseTimer::modifyExerciseInSet(int setIndex, int exerciseIndex)
{
    FUTR();
    if (setIndex < 0 || setIndex >= mModel->count())
    {
        return;
    }
    ExerciseSet *set = mModel->at(setIndex);
    if (exerciseIndex < 0 || exerciseIndex >= set->count())
    {
        return;
    }
    TRACE2("modification of exercise %1 in set %2 requested", exerciseIndex, setIndex);
    emit requestModificationOfExercise(set->at(exerciseIndex));

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

void ExerciseTimer::onTotalDurationChanged(int newTotalDurationSeconds)
{
    mTotalDuration = QTime(0, 0, 0).addSecs(newTotalDurationSeconds);
    emit totalDurationChanged(mTotalDuration);
    // This signal fires on every structural and duration/rounds-affecting
    // edit (add/remove, mins/secs/rounds changes at either level), making
    // it the one reliable place to keep the play sequence fresh even for
    // plain field edits that don't go through addSet()/removeSet().
    rebuildPlaySequence();
    emit playSequenceChanged();
    scheduleDraftSave();
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

QTime ExerciseTimer::currentDuration()
{
    return getExercise(mCurrentExerciseIndex)->duration();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
bool ExerciseTimer::running() const
{
    return mRunning;
}

bool ExerciseTimer::isPaused() const
{
    return mPaused;
}

bool ExerciseTimer::isWaitingToStart() const
{
    return mWaitingToStart;
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
        if (mPlayer && !mMuteSounds) mPlayer->playSound(SoundPlayer::RoundStartSound);
    }
    mCurrentExerciseDuration = QTime(0, ex->mins(), ex->secs());
    emit currentDurationChanged(mCurrentExerciseDuration);
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
    if (index < 0 || index >= mPlaySequence.size())
    {
        return 0;
    }
    return mPlaySequence.at(index).exercise;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::rebuildPlaySequence()
{
    mPlaySequence.clear();
    int setCount = mModel->count();
    for (int s = 0; s < setCount; ++s)
    {
        ExerciseSet *set = mModel->at(s);
        for (int setRound = 0; setRound < set->rounds(); ++setRound)
        {
            for (int e = 0; e < set->count(); ++e)
            {
                TimedExercise *exercise = set->at(e);
                for (int exerciseRound = 0; exerciseRound < exercise->rounds();
                     ++exerciseRound)
                {
                    PlayItem item;
                    item.exercise = exercise;
                    item.setIndex = s;
                    item.setCount = setCount;
                    item.setRoundNumber = setRound + 1;
                    item.setRoundCount = set->rounds();
                    item.exerciseRoundNumber = exerciseRound + 1;
                    item.exerciseRoundCount = exercise->rounds();
                    mPlaySequence.append(item);
                }
            }
        }
    }
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
        if (mPlaySequence.size() > mCurrentExerciseIndex + 1)
        {
            int nextIndex = mCurrentExerciseIndex + 1;
            bool nextIsLastRest = mSkipLastRest
                && nextIndex == mPlaySequence.size() - 1
                && getExercise(nextIndex)->activityType() == "rest";
            if (nextIsLastRest)
            {
                mCountdownTimer->stop();
                emit allExercisesFinished();
                mTotalRunningTime.setHMS(0, 0, 0);
                emit totalRunningTimeChanged(mTotalRunningTime);
            }
            else
            {
                // Check if the next activity is work or rest. If it is work,
                // do not play round end sound for the just ended round. Instead
                // the round start sound will be played. If the next round is rest,
                // play the round end sound.
                mCurrentExerciseIndex = nextIndex;
                if (getExercise(mCurrentExerciseIndex)->activityType() == "rest")
                {
                    if (mPlayer && !mMuteSounds) mPlayer->playSound(SoundPlayer::RoundEndSound);
                }
                TRACE1("Switching to activity %1", mCurrentExerciseIndex + 1);
                playCurrentExercise();
            }
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
    if (mPlayer && !mMuteSounds) mPlayer->playSound(SoundPlayer::AllDoneSound);
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
    if (mPlayer && !mMuteSounds && repNumber > 0) mPlayer->playSound(SoundPlayer::RepSound);
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
    if (!mWaitingToStart)
        return;
    mWaitingToStart = false;
    emit waitingToStartChanged(mWaitingToStart);
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

void ExerciseTimer::onSetValidityChanged(bool isValid)
{
    FUTR();
    Q_UNUSED(isValid);
    checkOverallValidity();
}

void ExerciseTimer::checkOverallValidity()
{
    bool allValid = mModel->count() > 0;
    for (int i = 0; allValid && i < mModel->count(); ++i)
    {
        if (!mModel->at(i)->isValid())
        {
            allValid = false;
        }
    }
    TRACE2("Set count: %1, allValid: %2", mModel->count(), allValid);
    if (mAllValid != allValid)
    {
        mAllValid = allValid;
        emit validityChanged(mAllValid);
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
bool ExerciseTimer::muteSounds() const
{
    return mMuteSounds;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::setMuteSounds(bool mute)
{
    FUTR();
    if (mute != mMuteSounds)
    {
        mMuteSounds = mute;
        emit muteSoundsChanged(mMuteSounds);
        QSettings settings;
        settings.setValue("mute sounds", mMuteSounds);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
bool ExerciseTimer::skipLastRest() const
{
    return mSkipLastRest;
}

void ExerciseTimer::setSkipLastRest(bool skip)
{
    if (skip != mSkipLastRest)
    {
        mSkipLastRest = skip;
        emit skipLastRestChanged(mSkipLastRest);
        QSettings settings;
        settings.setValue("skip last rest", mSkipLastRest);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
bool ExerciseTimer::showFirstUseHints() const
{
    return mShowFirstUseHints;
}

void ExerciseTimer::setShowFirstUseHints(bool show)
{
    if (show != mShowFirstUseHints)
    {
        mShowFirstUseHints = show;
        emit showFirstUseHintsChanged(mShowFirstUseHints);
    }
}

void ExerciseTimer::onCountDownForSound(int number)
{
    if (mPlayer && !mMuteSounds)
    {
        mPlayer->playCountDownSound(number);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
WorkoutHistory *ExerciseTimer::history() const
{
    return mHistory;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
ExerciseTemplateLibrary *ExerciseTimer::templateLibrary() const
{
    return mTemplateLibrary;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::onExerciseChangedForTemplate()
{
    TimedExercise *exercise = qobject_cast<TimedExercise *>(sender());
    if (!exercise || exercise->name().isEmpty() || !exercise->isValid())
    {
        return;
    }
    mTemplateLibrary->upsertTemplate(exercise->name(), exercise->mins(),
                                      exercise->secs(), exercise->reps());
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::addExerciseToSetFromTemplate(int setIndex, const QString &templateName)
{
    QVariantMap tmpl = mTemplateLibrary->templateByName(templateName);
    TimedExercise *exercise;
    if (tmpl.isEmpty())
    {
        exercise = new TimedExercise();
    }
    else
    {
        exercise = new TimedExercise("work", tmpl.value("mins").toInt(),
                                      tmpl.value("secs").toInt(),
                                      tmpl.value("reps").toInt());
        exercise->setName(templateName);
    }
    addExerciseToSet(setIndex, exercise, -1);
    // The copy is detached from here on (ADR-0008): its own later
    // duration/reps edits must never retroactively change the template
    // it was copied from. Renaming it is still a normal naming action
    // (e.g. under a different name, it upserts a new template).
    disconnect(exercise, &TimedExercise::minsChanged,
               this, &ExerciseTimer::onExerciseChangedForTemplate);
    disconnect(exercise, &TimedExercise::secsChanged,
               this, &ExerciseTimer::onExerciseChangedForTemplate);
    disconnect(exercise, &TimedExercise::repsChanged,
               this, &ExerciseTimer::onExerciseChangedForTemplate);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
QString ExerciseTimer::serializedDraft() const
{
    return QString::fromUtf8(
        QJsonDocument(mModel->toJson()).toJson(QJsonDocument::Compact));
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
bool ExerciseTimer::isDraftDirty() const
{
    return serializedDraft() != mLastSyncedDraftJson;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::markDraftSynced()
{
    bool wasDirty = isDraftDirty();
    mLastSyncedDraftJson = serializedDraft();
    if (wasDirty)
    {
        emit draftDirtyChanged(false);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
QVariantList ExerciseTimer::playSequenceSummary() const
{
    QVariantList sequence;
    for (const PlayItem &item : mPlaySequence)
    {
        QVariantMap occurrence;
        occurrence["activityType"] = item.exercise->activityType();
        occurrence["durationSeconds"] = item.exercise->durationSeconds();
        sequence.append(occurrence);
    }
    return sequence;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
QString ExerciseTimer::draftFilePath()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return QDir(dir).filePath("draft.json");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::loadModelFromJson(const QJsonArray &workoutJson)
{
    mModel->clear();
    mPlaySequence.clear();
    for (const QJsonValue &setVal : workoutJson)
    {
        QJsonObject setObj = setVal.toObject();
        ExerciseSet *set = new ExerciseSet();
        set->setRounds(setObj.value("rounds").toInt(1));
        addSet(set, -1);
        int setIndex = mModel->count() - 1;
        QJsonArray exercisesJson = setObj.value("exercises").toArray();
        for (const QJsonValue &exVal : exercisesJson)
        {
            addExerciseToSet(setIndex, TimedExercise::fromJson(exVal.toObject()), -1);
        }
    }
    checkOverallValidity();
    rebuildPlaySequence();
    emit playSequenceChanged();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::onExerciseChangedForDraft()
{
    emit playSequenceChanged();
    scheduleDraftSave();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::scheduleDraftSave()
{
    mDraftSaveTimer->start();
    bool dirty = isDraftDirty();
    emit draftDirtyChanged(dirty);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::saveDraftNow()
{
    QFile file(draftFilePath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.write(QJsonDocument(mModel->toJson()).toJson(QJsonDocument::Compact));
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
bool ExerciseTimer::saveDraftToHistory(const QString &name)
{
    if (!mAllValid)
    {
        return false;
    }
    mHistory->addEntry(name, mModel->toJson());
    markDraftSynced();
    return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::loadHistoryEntry(const QString &id)
{
    QJsonArray workoutJson = mHistory->workoutJson(id);
    if (workoutJson.isEmpty())
    {
        return;
    }
    loadModelFromJson(workoutJson);
    markDraftSynced();
    saveDraftNow();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void ExerciseTimer::resetDraftToDefault()
{
    mModel->clear();
    appendDefaultSet();
    markDraftSynced();
    saveDraftNow();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
TimedExercise * ExerciseTimer::currentActivity()
{
    return getExercise(mCurrentExerciseIndex);
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

bool ExerciseTimer::allExercisesValid() const
{
    return mAllValid;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
int ExerciseTimer::currentSetNumber() const
{
    if (mCurrentExerciseIndex < 0 || mCurrentExerciseIndex >= mPlaySequence.size())
    {
        return 0;
    }
    return mPlaySequence.at(mCurrentExerciseIndex).setIndex + 1;
}

int ExerciseTimer::currentSetCount() const
{
    if (mCurrentExerciseIndex < 0 || mCurrentExerciseIndex >= mPlaySequence.size())
    {
        return 0;
    }
    return mPlaySequence.at(mCurrentExerciseIndex).setCount;
}

int ExerciseTimer::currentSetRoundNumber() const
{
    if (mCurrentExerciseIndex < 0 || mCurrentExerciseIndex >= mPlaySequence.size())
    {
        return 0;
    }
    return mPlaySequence.at(mCurrentExerciseIndex).setRoundNumber;
}

int ExerciseTimer::currentSetRoundCount() const
{
    if (mCurrentExerciseIndex < 0 || mCurrentExerciseIndex >= mPlaySequence.size())
    {
        return 0;
    }
    return mPlaySequence.at(mCurrentExerciseIndex).setRoundCount;
}

int ExerciseTimer::currentExerciseRoundNumber() const
{
    if (mCurrentExerciseIndex < 0 || mCurrentExerciseIndex >= mPlaySequence.size())
    {
        return 0;
    }
    return mPlaySequence.at(mCurrentExerciseIndex).exerciseRoundNumber;
}

int ExerciseTimer::currentExerciseRoundCount() const
{
    if (mCurrentExerciseIndex < 0 || mCurrentExerciseIndex >= mPlaySequence.size())
    {
        return 0;
    }
    return mPlaySequence.at(mCurrentExerciseIndex).exerciseRoundCount;
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
