#include "exerciseset.h"
#include "timedexercise.h"

#include <algorithm>
#include <QJsonArray>

ExerciseSet::ExerciseSet(QObject *parent) : QObject(parent), mRounds(1),
    mWasValid(false)
{
}

int ExerciseSet::rounds() const
{
    return mRounds;
}

void ExerciseSet::setRounds(int rounds)
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

int ExerciseSet::count() const
{
    return mExercises.count();
}

bool ExerciseSet::isEmpty() const
{
    return mExercises.isEmpty();
}

TimedExercise *ExerciseSet::at(int i) const
{
    return mExercises.at(i);
}

bool ExerciseSet::isValid() const
{
    if (mExercises.isEmpty())
    {
        return false;
    }
    for (auto exercise : mExercises)
    {
        if (!exercise->isValid())
        {
            return false;
        }
    }
    return true;
}

int ExerciseSet::totalDurationSeconds() const
{
    int total = 0;
    for (auto exercise : mExercises)
    {
        total += exercise->durationSeconds() * exercise->rounds();
    }
    return total;
}

ExerciseSet *ExerciseSet::clone() const
{
    ExerciseSet *duplicate = new ExerciseSet();
    duplicate->setRounds(this->rounds());
    for (auto exercise : mExercises)
    {
        duplicate->appendExercise(exercise->clone());
    }
    return duplicate;
}

void ExerciseSet::appendExercise(TimedExercise *exercise)
{
    insertExercise(exercise, mExercises.size());
}

void ExerciseSet::insertExercise(TimedExercise *exercise, int position)
{
    if (position < 0 || position > mExercises.size())
    {
        return;
    }
    exercise->setParent(this);
    mExercises.insert(position, exercise);
    connect(exercise, &TimedExercise::durationChanged,
            this, &ExerciseSet::onChildDurationOrRoundsChanged);
    connect(exercise, &TimedExercise::roundsChanged,
            this, &ExerciseSet::onChildDurationOrRoundsChanged);
    connect(exercise, &TimedExercise::validityChanged,
            this, &ExerciseSet::onChildValidityChanged);
    emit countChanged();
    emit totalDurationChanged(totalDurationSeconds());
    onChildValidityChanged();
}

void ExerciseSet::removeExercise(int index)
{
    if (index < 0 || index >= mExercises.size())
    {
        return;
    }
    auto exerciseToRemove = mExercises.takeAt(index);
    exerciseToRemove->deleteLater();
    emit countChanged();
    emit totalDurationChanged(totalDurationSeconds());
    onChildValidityChanged();
}

void ExerciseSet::onChildDurationOrRoundsChanged()
{
    emit totalDurationChanged(totalDurationSeconds());
}

void ExerciseSet::onChildValidityChanged()
{
    bool valid = isValid();
    if (valid != mWasValid)
    {
        mWasValid = valid;
        emit validityChanged(mWasValid);
    }
}

QJsonObject ExerciseSet::toJson() const
{
    QJsonObject json;
    json["rounds"] = mRounds;
    QJsonArray exercises;
    for (auto exercise : mExercises)
    {
        exercises.append(exercise->toJson());
    }
    json["exercises"] = exercises;
    return json;
}

void ExerciseSet::moveItems(QList<int> selectedIndices, int targetIndex)
{
    if (selectedIndices.isEmpty() || targetIndex < 0 || targetIndex > mExercises.count())
        return;

    std::sort(selectedIndices.begin(), selectedIndices.end(), std::greater<int>());

    for (int sourceIdx : selectedIndices)
    {
        if (sourceIdx < 0 || sourceIdx >= mExercises.count() || sourceIdx == targetIndex)
            continue;

        auto itemToMove = mExercises.takeAt(sourceIdx);

        int actualTarget = targetIndex;
        if (sourceIdx < targetIndex)
        {
            actualTarget--;
        }

        mExercises.insert(actualTarget, itemToMove);
    }
}

void ExerciseSet::copyItems(QList<int> selectedIndices, int targetIndex)
{
    if (selectedIndices.isEmpty() || targetIndex < 0 || targetIndex > mExercises.count())
        return;

    std::sort(selectedIndices.begin(), selectedIndices.end(), std::greater<int>());

    for (int sourceIdx : selectedIndices)
    {
        if (sourceIdx < 0 || sourceIdx >= mExercises.count())
            continue;

        auto originalItem = mExercises.at(sourceIdx);
        auto copiedItem = originalItem->clone();
        insertExercise(copiedItem, targetIndex);
    }
}
