#include "exerciselistmodel.h"
#include "timedexercise.h"

ExerciseListModel::ExerciseListModel(QObject *parent) : QAbstractListModel(parent)
{

}

int ExerciseListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mExercises.size();
}

QVariant ExerciseListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= mExercises.size())
            return QVariant();

        if (role == ObjectRole)
            return QVariant::fromValue(mExercises.at(index.row()));

        return QVariant();
}

QHash<int, QByteArray> ExerciseListModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[ObjectRole] = "object";
    return roleNames;
}

void ExerciseListModel::appendExercise(TimedExercise *exercise)
{
    insertExercise(exercise, mExercises.size());
}

void ExerciseListModel::insertExercise(TimedExercise *exercise, int position)
{
    if (position >= 0 || position <= mExercises.size())
    {
        beginInsertRows(QModelIndex(), position, position);
        mExercises.insert(position, exercise);
        endInsertRows();
        const auto durationSeconds = exercise->durationSeconds();
        mTotalDurationSeconds += durationSeconds;
        emit countChanged();
        emit totalDurationChanged(durationSeconds);
        connect(exercise, &TimedExercise::durationChanged, this,
                &ExerciseListModel::exerciseDurationChanged);
    }
}

void ExerciseListModel::removeExercise(int index)
{
    if (index < 0 || index >= mExercises.size()) {
            return;
        }
        beginRemoveRows(QModelIndex(), index, index);
        const auto exerciseToRemove = mExercises.takeAt(index);
        endRemoveRows();
        const auto durationSeconds = exerciseToRemove->durationSeconds();
        mTotalDurationSeconds -= durationSeconds;
        emit countChanged();
        emit totalDurationChanged(-durationSeconds);
}

void ExerciseListModel::clear()
{
    if (mExercises.isEmpty()) return;
    beginRemoveRows(QModelIndex(), 0, mExercises.size() - 1);
    mExercises.clear();
    endRemoveRows();
    emit countChanged();
    emit totalDurationChanged(-mTotalDurationSeconds);
    mTotalDurationSeconds = 0;

}

void ExerciseListModel::exerciseDurationChanged(int durationChangeSeconds)
{
    mTotalDurationSeconds += durationChangeSeconds;
    emit totalDurationChanged(durationChangeSeconds);
}

/*
bool ExerciseListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // if successful
    emit dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, , const QVector<int> &roles = QVector<int> ());
    return true;
}*/
