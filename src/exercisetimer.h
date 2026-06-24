/*! ExerciseTimer class does the busines logic of an exercise timer.
A commercial example of such a timer is GymBoss. ExerciseTimer manages a list
of timed exercises (defined in class TimedExercise) and rest periods. It has
methods to start and stop the activities. It notifies the ui for starting,
 stopping, countdown and repetition events. */


#ifndef EXERCISETIMER_H
#define EXERCISETIMER_H

#include <QObject>
#include <QTime>
#include <QVector>
#include <QJsonArray>
#include <QVariantList>

class QTimer;

class TimedExercise;
class ExerciseSet;
class ExerciseListModel;
class SoundPlayer;
class WorkoutHistory;
class ExerciseTemplateLibrary;

class ExerciseTimer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QTime totalDuration READ totalDuration NOTIFY
               totalDurationChanged)
    Q_PROPERTY(QTime currentRunningTime READ currentRunningTime NOTIFY
               currentRunningTimeChanged)
    Q_PROPERTY(QTime currentDuration READ currentDuration NOTIFY
               currentDurationChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningStatusChanged)
    Q_PROPERTY(int startDelay READ startDelay WRITE setStartDelay
               NOTIFY startDelayChanged)
    Q_PROPERTY(int endWarningTime READ endWarningTime WRITE setEndWarningTime
               NOTIFY endWarningTimeChanged)
    Q_PROPERTY(bool muteSounds READ muteSounds WRITE setMuteSounds
               NOTIFY muteSoundsChanged)
    Q_PROPERTY(QTime totalRunningTime READ totalRunningTime NOTIFY
               totalRunningTimeChanged)
    Q_PROPERTY(TimedExercise* currentActivity READ currentActivity NOTIFY
               currentActivityChanged)
    Q_PROPERTY(int currentRepNumber READ currentRepNumber NOTIFY notifyRep)
    Q_PROPERTY(double currentProgress READ currentProgress NOTIFY
               currentProgressChanged)
    Q_PROPERTY(double totalProgress READ totalProgress NOTIFY
               totalProgressChanged)
    Q_PROPERTY(bool allValid READ allExercisesValid NOTIFY validityChanged)
    Q_PROPERTY(int currentSetNumber READ currentSetNumber NOTIFY currentActivityChanged)
    Q_PROPERTY(int currentSetCount READ currentSetCount NOTIFY currentActivityChanged)
    Q_PROPERTY(int currentSetRoundNumber READ currentSetRoundNumber
               NOTIFY currentActivityChanged)
    Q_PROPERTY(int currentSetRoundCount READ currentSetRoundCount
               NOTIFY currentActivityChanged)
    Q_PROPERTY(int currentExerciseRoundNumber READ currentExerciseRoundNumber
               NOTIFY currentActivityChanged)
    Q_PROPERTY(int currentExerciseRoundCount READ currentExerciseRoundCount
               NOTIFY currentActivityChanged)
    Q_PROPERTY(WorkoutHistory* history READ history CONSTANT)
    Q_PROPERTY(ExerciseTemplateLibrary* templateLibrary READ templateLibrary CONSTANT)
    /*! True when the Draft differs from its own last-saved/loaded
    History state (see ADR-0011). */
    Q_PROPERTY(bool draftDirty READ isDraftDirty NOTIFY draftDirtyChanged)
    /*! The Draft's play sequence flattened to one entry per Exercise
    occurrence, for the editor's Summary Bar (see ADR-0013). */
    Q_PROPERTY(QVariantList playSequenceSummary READ playSequenceSummary
               NOTIFY playSequenceChanged)

public:
    /*! Construct a new ExerciseTimer object.
    @param enableSound If false, no SoundPlayer is created. Used by tests
    to avoid depending on an audio backend. */
    explicit ExerciseTimer(QObject *parent = 0, bool enableSound = true);
    ExerciseListModel* exerciseListModel();
    /*! The sum of durations of all activities in the list */
    QTime totalDuration() const;
    /*! Running time of the current activity */
    QTime currentRunningTime() const;
    QTime currentDuration();
    bool running() const;
    /*! Time after which the exercises really start*/
    int startDelay() const;
    /*! Time, in seconds, when a warning before the end of the activity is
    sent*/
    int endWarningTime() const;
    /*! True if all sound effects (countdown, round-start/end, rep,
    all-done) are suppressed */
    bool muteSounds() const;
    /*! The cumulative running time of all activities*/
    QTime totalRunningTime() const;
    /*! Return a pointer to the current activity */
    TimedExercise* currentActivity();
    /*! Return the number of repetitions for current exercise*/
    int currentRepNumber() const;
    /*! Progress of current activity from 0 to 1 */
    double currentProgress() const;
    /*! Progress of total activity from 0 to 1 */
    double totalProgress() const;
    bool allExercisesValid() const;
    /*! Which set (1-based) the currently playing exercise belongs to */
    int currentSetNumber() const;
    /*! Total number of sets in the workout */
    int currentSetCount() const;
    /*! Which round (1-based) of the current set is currently playing */
    int currentSetRoundNumber() const;
    /*! Total number of rounds configured for the current set */
    int currentSetRoundCount() const;
    /*! Which round (1-based) of the current exercise is currently playing */
    int currentExerciseRoundNumber() const;
    /*! Total number of rounds configured for the current exercise */
    int currentExerciseRoundCount() const;
    WorkoutHistory* history() const;
    ExerciseTemplateLibrary* templateLibrary() const;
    bool isDraftDirty() const;
    QVariantList playSequenceSummary() const;
    /*! Path the Draft is auto-persisted to. Exposed so tests can isolate
    themselves from the real per-user draft file. */
    static QString draftFilePath();

signals:
    void totalDurationChanged(QTime duration);
    void requestModificationOfExercise(TimedExercise* exercise);
    void currentRunningTimeChanged(QTime currentRunningTime);
    void currentDurationChanged(QTime currentDuration);
    void runningStatusChanged(bool running);
    void currentExerciseFinished();
    void allExercisesFinished();
    /*! Emitted when a repetition should be done */
    void notifyRep(int repetitionNumber);
    void startDelayChanged(int seconds);
    void endWarningTimeChanged(int seconds);
    void muteSoundsChanged(bool muted);
    /*! Emitted when the current activity is close to its end,
    according to mEndNotificationTime */
    void currentExerciseCloseToEnd();
    void totalRunningTimeChanged(QTime totalRunningTime);
    /*! Emitted during countdown.
    @param number is the current second in countdown */
    void countDown(int number);
    void currentActivityChanged(TimedExercise* currentActivity);
    void currentProgressChanged(double progress);
    void totalProgressChanged(double progress);
    void validityChanged(bool allValid);
    void draftDirtyChanged(bool dirty);
    void playSequenceChanged();

public slots:
    void addSet(ExerciseSet* set, int pos = -1);
    /*! Appends a new set, pre-populated with one default exercise so it is
    immediately valid */
    void appendDefaultSet();
    void removeSet(int index);
    void addExerciseToSet(int setIndex, TimedExercise* exercise, int pos = -1);
    /*! Adds a new default (work, 1 min) exercise to the given set; used by
    the set's own "+" button in the editor */
    void addDefaultExerciseToSet(int setIndex);
    void addRestToSet(int setIndex, int mins = 1, int secs = 0);
    /*! Adds a new Exercise to the given Set, copying its initial
    name/duration/reps by value from the named template (ADR-0008). If
    no such template exists, behaves like addDefaultExerciseToSet(). */
    void addExerciseToSetFromTemplate(int setIndex, const QString &templateName);
    void removeExerciseFromSet(int setIndex, int exerciseIndex);
    void modifyExerciseInSet(int setIndex, int exerciseIndex);
    /*! Start the exercise sequence, or resume playing if it was
    paused. */
    void start();
    void pause();
    /*! Stops the timer and sets currentRunningTime and totalRunningTime
    to zero, and makes the first activity the current one. Thus, if the clock
    is started again, the activities start from the beginning. */
    void reset();
    /*! Set the  number of seconds for a countdown timer */
    void setStartDelay(int seconds);
    void setEndWarningTime(int seconds);
    void setMuteSounds(bool mute);
    /*! Saves a snapshot of the Draft into History under the given name.
    Requires the Draft to currently be allValid (ADR-0012); returns false
    and does nothing otherwise. On success, the Draft is marked as
    synced with the new entry (draftDirty becomes false) but is
    otherwise left open and unchanged (ADR-0006). */
    bool saveDraftToHistory(const QString &name);
    /*! Replaces the Draft with the given History entry's workout
    (ADR-0011). No-op if no such entry exists. */
    void loadHistoryEntry(const QString &id);
    /*! Marks the Draft's current state as the synced baseline draftDirty
    compares against - i.e. "no unsaved changes from here". Called
    internally after loading/saving the Draft; also called by main()
    after seeding a brand new install's default Set, so a never-touched
    fresh install doesn't read as dirty. */
    void markDraftSynced();


private slots:
    /*! Connected to change in the number of exercises in the list */
    void onCountChanged();
    void onTotalDurationChanged(int totalDurationChangeSeconds);
    void onCurrentRunningTimeChanged(QTime runTime);
    void onTotalRunningTimeChanged(QTime totalRunTime);
    void playCurrentExercise();
    void onCurrentExerciseFinished();
    void onAllExercisesFinished();
    void onRunningStatusChanged(bool running);
    void doRepetition(int repNumber);
    void startAfterDelay();
    void onCurrentExerciseCloseToEnd();
    void startCountDown(int durationSeconds);
    void onCountDown();
    void onSetValidityChanged(bool isValid);
    void checkOverallValidity();
    /*! Forwards countDown to the SoundPlayer, unless sounds are muted */
    void onCountDownForSound(int number);
    /*! Connected to any per-Exercise change that doesn't already flow
    through mModel's totalDurationChanged (activityType, reps): keeps
    the Draft's play-sequence summary and on-disk copy in sync. */
    void onExerciseChangedForDraft();
    /*! Connected to a per-Exercise name/duration/reps change: upserts
    the exercise template library entry matching the exercise's current
    name (ADR-0009). No-op if the exercise is currently unnamed. */
    void onExerciseChangedForTemplate();
    /*! Debounced write of the Draft to draftFilePath() */
    void saveDraftNow();

private: // types

    /*! One play of one exercise within the flattened play sequence,
    together with the position metadata (which set, which exercise round)
    needed to drive the RunPage position display. */
    struct PlayItem
    {
        TimedExercise* exercise;
        int setIndex;
        int setCount;
        int setRoundNumber;
        int setRoundCount;
        int exerciseRoundNumber;
        int exerciseRoundCount;
    };

private: //data

    /*! The total duration of exercises and rest periods in the list*/
    QTime mTotalDuration;
    /*! The set duration the current exercise or rest period */
    QTime mCurrentExerciseDuration;
    /*! The index of the currently running exercise or rest period*/
    int mCurrentExerciseIndex;
    /*! A model that holds the activities and serves as a model for a QML
    ListView */
    ExerciseListModel* mModel;
    /*! The total running time. Starts from zero and increases when
    the exercises go on. */
    QTime mTotalRunningTime;
    /*! The running time of the current exercise. Starts from zero.
    When it reaches the duration of the exercise, the exercise stops.*/
    QTime mCurrentRunningTime;
    /*! A time that is restarted at every timer event, and whose
    elapsed time is always added to the running time */
    QTime mStartTime;
    /*! Interval of timer events in milliseconds */
    int mTimerInterval;
    /*! Identifier of the timer */
    int mTimerID;
    /*! Number of seconds before the timer will really start after
    invoking the start method. Also known as lead-in time. Default 5 s.*/
    int mStartDelay;
    /*! True when running, false when stopped or paused */
    bool mRunning;
    /*! True if the startDelay period is running  */
    bool mWaitingToStart;

    /*! True if events for repetitions are sent (e.g. a sound is played) */
    bool mNotifyReps;
    /*! Rep notification interval in milliseconds. It depends on the
    exercise's duration and the number of repetitions. */
    int mRepSeparationMilliSecs;
    /*! Used in setting rep delay timer, see the source*/
    bool mCheckRepTimer;
    /*! How many seconds before the end of the exercise a notification is
    dispatched (which results in e.g. playing a sound). Default 3 seconds. */
    int mEndNotificationTime;
    /*! True if all sound effects are suppressed */
    bool mMuteSounds;
    /*! The duration after starting the current exercise, after which
    the end notification should be sent*/
    QTime mCurrentExerciseEndNotificationTime;
    /*! True if the end end notification should be sent for the current
    activity period, false otherwise. It is false also if it already
    has been sent. */
    bool mSendEndNotification;
    /*! A sound player derived from QMediaPlayer */
    SoundPlayer* mPlayer;
    /*! A timer used for countdown */
    QTimer* mCountdownTimer;
    int mCountdownTracker;
    /*! Stores the number of reps done for the current exercise*/
    int mCurrentRepNumber;
    /*! Progress of current activity from 0 to 1 */
    double mCurrentProgress;
    /*! Progress of total activity from 0 to 1 */
    double mTotalProgress;
    bool mAllValid;
    /*! The flattened sequence of plays derived from the Set/Round tree;
    rebuilt whenever the workout structure changes. mCurrentExerciseIndex
    indexes into this, not directly into mModel. */
    QVector<PlayItem> mPlaySequence;
    /*! The persisted workout History (ADR-0005/0006) */
    WorkoutHistory* mHistory;
    /*! The persisted exercise template library (ADR-0008) */
    ExerciseTemplateLibrary* mTemplateLibrary;
    /*! Compact JSON serialization of the Draft as of the last time it
    was loaded, saved, or otherwise made to match a known History state.
    isDraftDirty() compares the Draft's current serialization against
    this. */
    QString mLastSyncedDraftJson;
    /*! Debounces saveDraftNow() so rapid edits don't write to disk on
    every keystroke. */
    QTimer* mDraftSaveTimer;

private: // methods
    /*! Return the pointer of the exercise at index i in mPlaySequence */
    TimedExercise* getExercise(int index);
    /*! Rebuild mPlaySequence by walking the Set/Round tree */
    void rebuildPlaySequence();
    /*! Replaces the Draft's entire Set/Exercise tree from JSON produced
    by ExerciseListModel::toJson(), reusing addSet()/addExerciseToSet()
    so all the usual wiring (validity, play sequence, draft-save
    connections) is set up identically to interactive edits. */
    void loadModelFromJson(const QJsonArray &workoutJson);
    /*! Compact JSON serialization of the Draft's current state. */
    QString serializedDraft() const;
    /*! (Re)starts the debounce timer that calls saveDraftNow(). */
    void scheduleDraftSave();

protected:
    void timerEvent(QTimerEvent *);


};

#endif // EXERCISETIMER_H
