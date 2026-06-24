#include "exerciselistmodel.h"
#include "exerciseset.h"
#include "eoqttrace.h"

#include <algorithm>

ExerciseListModel::ExerciseListModel(QObject *parent) : QAbstractListModel(parent)
{

}

int ExerciseListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mSets.size();
}

QVariant ExerciseListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= mSets.size())
            return QVariant();

        if (role == SetRole)
            return QVariant::fromValue(mSets.at(index.row()));

        return QVariant();
}

QHash<int, QByteArray> ExerciseListModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[SetRole] = "set";
    return roleNames;
}

int ExerciseListModel::totalDurationSeconds() const
{
    int total = 0;
    for (auto set : mSets)
    {
        total += set->totalDurationSeconds() * set->rounds();
    }
    return total;
}

QJsonArray ExerciseListModel::toJson() const
{
    QJsonArray array;
    for (auto set : mSets)
    {
        array.append(set->toJson());
    }
    return array;
}

void ExerciseListModel::appendSet(ExerciseSet *set)
{
    insertSet(set, mSets.size());
}

void ExerciseListModel::insertSet(ExerciseSet *set, int position)
{
    if (position >= 0 && position <= mSets.size())
    {
        beginInsertRows(QModelIndex(), position, position);
        set->setParent(this);
        mSets.insert(position, set);
        endInsertRows();
        emit countChanged();
        emit totalDurationChanged(totalDurationSeconds());
        connect(set, &ExerciseSet::totalDurationChanged, this,
                &ExerciseListModel::onSetChanged);
        connect(set, &ExerciseSet::roundsChanged, this,
                &ExerciseListModel::onSetChanged);
    }
}

void ExerciseListModel::removeSet(int index)
{
    FUNC_TRACE(QString("Removing set with index %1").arg(index));
    if (index < 0 || index >= mSets.size()) {
            return;
        }
        beginRemoveRows(QModelIndex(), index, index);
        const auto setToRemove = mSets.takeAt(index);
        endRemoveRows();
        emit countChanged();
        emit totalDurationChanged(totalDurationSeconds());
        setToRemove->deleteLater();
}

void ExerciseListModel::clear()
{
    if (mSets.isEmpty()) return;
    beginRemoveRows(QModelIndex(), 0, mSets.size() - 1);
    for (auto set : mSets)
    {
        set->deleteLater();
    }
    mSets.clear();
    endRemoveRows();
    emit countChanged();
    emit totalDurationChanged(0);

}

void ExerciseListModel::moveItems(QList<int> selectedIndices, int targetIndex)
{
    if (selectedIndices.isEmpty() || targetIndex < 0 || targetIndex > mSets.count())
            return;

        std::sort(selectedIndices.begin(), selectedIndices.end(), std::greater<int>());

        for (int sourceIdx : selectedIndices) {
            if (sourceIdx < 0 || sourceIdx >= mSets.count() || sourceIdx == targetIndex)
                continue;

            int qtTarget = (sourceIdx < targetIndex) ? targetIndex + 1 : targetIndex;

            if (beginMoveRows(QModelIndex(), sourceIdx, sourceIdx, QModelIndex(), qtTarget)) {

                auto itemToMove = mSets.takeAt(sourceIdx);

                int actualTarget = targetIndex;
                if (sourceIdx < targetIndex) {
                    actualTarget--;
                }

                mSets.insert(actualTarget, itemToMove);
                endMoveRows();
            }
        }

}

void ExerciseListModel::copyItems(QList<int> selectedIndices, int targetIndex)
{
    if (selectedIndices.isEmpty() || targetIndex < 0 || targetIndex > mSets.count())
            return;

        std::sort(selectedIndices.begin(), selectedIndices.end(), std::greater<int>());

        for (int sourceIdx : selectedIndices) {
            if (sourceIdx < 0 || sourceIdx >= mSets.count())
                continue;

            beginInsertRows(QModelIndex(), targetIndex, targetIndex);

            auto originalItem = mSets.at(sourceIdx);
            auto copiedItem = originalItem->clone();
            copiedItem->setParent(this);
            mSets.insert(targetIndex, copiedItem);
            connect(copiedItem, &ExerciseSet::totalDurationChanged, this,
                    &ExerciseListModel::onSetChanged);
            connect(copiedItem, &ExerciseSet::roundsChanged, this,
                    &ExerciseListModel::onSetChanged);

            endInsertRows();
        }
}

void ExerciseListModel::onSetChanged()
{
    emit totalDurationChanged(totalDurationSeconds());
}
