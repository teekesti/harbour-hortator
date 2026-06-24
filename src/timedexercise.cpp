#include "timedexercise.h"
#include "eoqttrace.h"

#include<QTime>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
TimedExercise::TimedExercise(QString activityType, int mins,
                             int secs, int reps, QObject *parent) :
    QObject(parent), mActivityType(activityType), mMins(mins), mSecs(secs),
    mReps(reps), mRounds(1)
{
    updateRPM();
}

TimedExercise *TimedExercise::clone() const
{
    TimedExercise *duplicate = new TimedExercise();
    duplicate->setParent(this->parent());
    duplicate->setActivityType(this->activityType());
    duplicate->setName(this->name());
    duplicate->setMins(this->mins());
    duplicate->setSecs(this->secs());
    duplicate->setReps(this->reps());
    duplicate->setRounds(this->rounds());
    return duplicate;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
int TimedExercise::mins() const
{
    return mMins;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
int TimedExercise::secs() const
{
    return mSecs;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
int TimedExercise::reps() const
{
    return mReps;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
int TimedExercise::rounds() const
{
    return mRounds;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
double TimedExercise::rpm() const
{
    return mRPM;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
double TimedExercise::repSeparation() const
{
    return mRepSeparation;
}

QTime TimedExercise::duration() const
{
    return QTime(0, mMins, mSecs);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void TimedExercise::setMins(int mins)
{
    if (mins != mMins)
    {
        int durationChangeSeconds = 60 * (mins - mMins);
        mMins = mins;
        TRACE1("New mins: %1", mMins);
        emit minsChanged(mins);
        emit durationChanged(durationChangeSeconds);
        updateRPM();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void TimedExercise::setSecs(int secs)
{
    if (secs != mSecs)
    {
        int durationChangeSeconds = secs - mSecs;
        mSecs = secs;
        TRACE1("New secs: %1", mSecs);
        emit secsChanged(mSecs);
        emit durationChanged(durationChangeSeconds);
        updateRPM();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void TimedExercise::setReps(int reps)
{
    if (reps != mReps)
    {
        mReps = reps;
        emit repsChanged(mReps);
        updateRPM();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void TimedExercise::setRounds(int rounds)
{
    if (rounds < 1)
    {
        rounds = 1;
    }
    if (rounds != mRounds)
    {
        mRounds = rounds;
        emit roundsChanged(mRounds);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
int TimedExercise::durationSeconds() const
{
    return mSecs + 60 * mMins;
}

bool TimedExercise::isValid() const
{
    return mMins > 0 || mSecs > 0;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
double TimedExercise::durationMinutes() const
{
    return mMins + (double)mSecs/60.0;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void TimedExercise::updateRPM()
{
    // repetitions per minute
    double rpm(0);
    // delay between reps in seconds
    double delay(0);
    if ((0 != durationMinutes()) && (0 != mReps))
    {
        rpm = mReps/durationMinutes();
        delay = (double)durationSeconds()/mReps;

    }
    if (mRPM != rpm)
    {
        mRPM = rpm;
        emit rpmChanged(mRPM);
    }
    if (mRepSeparation != delay)
    {
        mRepSeparation = delay;
        emit repSeparationChanged(mRepSeparation);
    }
    bool valid = isValid();
    if (mIsValid != valid)
    {
        mIsValid = valid;
        emit validityChanged(mIsValid);
        TRACE1("New validity: %1", mIsValid);
    }

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
QString TimedExercise::activityType() const
{
    return mActivityType;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void TimedExercise::setActivityType(QString activityType)
{
    if (activityType != mActivityType)
    {
        mActivityType = activityType;
        emit activityTypeChanged(mActivityType);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
QString TimedExercise::name() const
{
    return mName;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void TimedExercise::setName(QString name)
{
    if (name != mName)
    {
        mName = name;
        emit nameChanged(mName);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
void TimedExercise::toggleActivityType()
{
    if (mActivityType == "work")
    {
        setActivityType("rest");
    }
    else
    {
        setActivityType("work");
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
QJsonObject TimedExercise::toJson() const
{
    QJsonObject json;
    json["activityType"] = mActivityType;
    json["name"] = mName;
    json["mins"] = mMins;
    json["secs"] = mSecs;
    json["reps"] = mReps;
    json["rounds"] = mRounds;
    return json;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
TimedExercise *TimedExercise::fromJson(const QJsonObject &json)
{
    TimedExercise *exercise = new TimedExercise(
        json.value("activityType").toString("work"),
        json.value("mins").toInt(1),
        json.value("secs").toInt(0),
        json.value("reps").toInt(0));
    exercise->setName(json.value("name").toString());
    exercise->setRounds(json.value("rounds").toInt(1));
    return exercise;
}


