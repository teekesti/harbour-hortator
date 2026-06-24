#ifndef TIMEDEXERCISE_H
#define TIMEDEXERCISE_H

#include <QObject>
#include <QMetaType>
#include <QJsonObject>

class TimedExercise : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString activityType READ activityType WRITE setActivityType
               NOTIFY activityTypeChanged)
    Q_PROPERTY(int mins READ mins WRITE setMins NOTIFY minsChanged)
    Q_PROPERTY(int secs READ secs WRITE setSecs NOTIFY secsChanged)
    Q_PROPERTY(int reps READ reps WRITE setReps NOTIFY repsChanged)
    Q_PROPERTY(int rounds READ rounds WRITE setRounds NOTIFY roundsChanged)
    Q_PROPERTY(double rpm READ rpm NOTIFY rpmChanged)
    Q_PROPERTY(double repSeparation READ repSeparation
               NOTIFY repSeparationChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY validityChanged)
public:
    explicit TimedExercise(QString activityType = "work", int mins = 1,
                           int secs = 0, int reps = 0, QObject *parent = 0);
    TimedExercise* clone() const;
    QString activityType() const;
    /*! Return the minutes part of the duration of the exercise */
    int mins() const;
    /*! Return the seconds part of the duration of the exercise */
    int secs() const;
    /*! Return the planned number of repetitions */
    int reps() const;
    /*! Return how many times this exercise is replayed before moving on */
    int rounds() const;
    /*! Return the number of repetitions per minute */
    double rpm() const;
    /*! Return the time in seconds between repetitions */
    double repSeparation() const;
    /*! Duration as QTime */
    QTime duration() const;
    /*! Returns the duration in seconds.*/
    int durationSeconds() const;
    bool isValid() const;
    /*! Serializes this exercise's persisted fields (activity type,
    duration, reps, rounds) to JSON, for the Draft/History stores. */
    QJsonObject toJson() const;
    /*! Constructs a new TimedExercise from JSON previously produced by
    toJson(). Missing fields fall back to the same defaults as the
    default constructor. */
    static TimedExercise *fromJson(const QJsonObject &json);

signals:
    void activityTypeChanged(QString activityType);
    void minsChanged(int mins);
    void secsChanged(int secs);
    void durationChanged(int durationChangeSeconds);
    void repsChanged(int reps);
    void roundsChanged(int rounds);
    void rpmChanged(double rpm);
    void repSeparationChanged(double repSeparation);
    void validityChanged(bool isValid);

public slots:
    void setActivityType(QString activityType);
    void toggleActivityType();
    void setMins(int mins);
    void setSecs(int secs);
    void setReps(int reps);
    void setRounds(int rounds);

private slots:
    void updateRPM();

private: // data
    QString mActivityType;
    int mMins;
    int mSecs;
    int mReps;
    int mRounds;
    double mRPM;
    double mRepSeparation;
    bool mIsValid;

private: // methods

    double durationMinutes() const;

};

#endif // TIMEDEXERCISE_H
