/*! ExerciseTimer class does the busines logic of an exercise timer.
A commercial example of such a timer is GymBoss. ExerciseTimer manages a list
of timed exercises (defined in class TimedExercise) and rest periods. It has
methods to start and stop the activities. It notifies the ui for starting,
 stopping, countdown and repetition events. */


#ifndef EXERCISETIMER_H
#define EXERCISETIMER_H

#include <QObject>
#include <QTime>

class QTimer;

class TimedExercise;
class ExerciseListModel;
class SoundPlayer;

class ExerciseTimer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QTime totalDuration READ totalDuration NOTIFY
               totalDurationChanged)
    Q_PROPERTY(QTime currentRunningTime READ currentRunningTime NOTIFY
               currentRunningTimeChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningStatusChanged)
    Q_PROPERTY(int startDelay READ startDelay WRITE setStartDelay
               NOTIFY startDelayChanged)
    Q_PROPERTY(int endWarningTime READ endWarningTime WRITE setEndWarningTime
               NOTIFY endWarningTimeChanged)
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
    //Q_PROPERTY(ExerciseListModel* exerciseListModel READ exerciseListModel)

public:
    /*! Construct a new ExerciseTimer object*/
    explicit ExerciseTimer(QObject *parent = 0);
    ExerciseListModel* exerciseListModel();
    /*! The sum of durations of all activities in the list */
    QTime totalDuration() const;
    /*! Running time of the current activity */
    QTime currentRunningTime() const;
    bool running() const;
    /*! Time after which the exercises really start*/
    int startDelay() const;
    /*! Time, in seconds, when a warning before the end of the activity is
    sent*/
    int endWarningTime() const;
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

signals:
    void totalDurationChanged(QTime duration);
    void requestModificationOfExercise(TimedExercise* exercise);
    void currentRunningTimeChanged(QTime currentRunningTime);
    void runningStatusChanged(bool running);
    void currentExerciseFinished();
    void allExercisesFinished();
    /*! Emitted when a repetition should be done */
    void notifyRep(int repetitionNumber);
    void startDelayChanged(int seconds);
    void endWarningTimeChanged(int seconds);
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

public slots:
    void addExercise(TimedExercise* exercise, int pos = -1);
    void appendDefaultExercise();
    void addRest(int mins = 1, int secs = 0);
    void modifyExercise(int index);
    void removeExercise(int index);
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
    void exerciseValidityChanged(bool isValid);
    void checkOverallValidity();

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
    /*! Count of valid exercises to check if all are valid */
    int mValidExerciseCount;
    bool mAllValid;
    /*! A system screen saver object for disabling screen saver during action,
    if so desired. */
    //QSystemScreenSaver* mScreenSaver;

private: // methods
    /*! Return the pointer of the exercise at index i, cast to
    TimedExercise type */
    TimedExercise* getExercise(int index);

protected:
    void timerEvent(QTimerEvent *);


};

#endif // EXERCISETIMER_H
