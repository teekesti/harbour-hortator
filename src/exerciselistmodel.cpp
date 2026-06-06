#include "exerciselistmodel.h"
#include "timedexercise.h"
#include "eoqttrace.h"

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

        if (role == ExerciseRole)
            return QVariant::fromValue(mExercises.at(index.row()));

        return QVariant();
}

QHash<int, QByteArray> ExerciseListModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[ExerciseRole] = "exercise";
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
    FUNC_TRACE(QString("Removing exercise with index %1").arg(index));
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
        exerciseToRemove->deleteLater();
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

void ExerciseListModel::moveItems(QList<int> selectedIndices, int targetIndex)
{
    if (selectedIndices.isEmpty() || targetIndex < 0 || targetIndex > mExercises.count())
            return;

        // 1. Lajitellaan valitut indeksit SUURIMMASTA PIENIMPÄÄN
        std::sort(selectedIndices.begin(), selectedIndices.end(), std::greater<int>());

        // 2. Suoritetaan siirrot yksitellen turvallisessa järjestyksessä
        for (int sourceIdx : selectedIndices) {
            if (sourceIdx < 0 || sourceIdx >= mExercises.count() || sourceIdx == targetIndex)
                continue;

            // Qt vaatii tiedon siitä, mihin kohteeseen rivi päätyy siirron jälkeen.
            // Jos siirretään alaspäin, kohdeindeksi kasvaa yhdellä sisäisesti Qt:n sääntöjen vuoksi.
            int qtTarget = (sourceIdx < targetIndex) ? targetIndex + 1 : targetIndex;

            // Ilmoitetaan QML-näkymälle siirron alkamisesta, jotta animaatiot toimivat oikein
            if (beginMoveRows(QModelIndex(), sourceIdx, sourceIdx, QModelIndex(), qtTarget)) {

                auto itemToMove = mExercises.takeAt(sourceIdx);

                // Päivitetään kohdeindeksi, jos oma poisto vaikutti sen sijaintiin QVectorissa
                int actualTarget = targetIndex;
                if (sourceIdx < targetIndex) {
                    actualTarget--;
                }

                mExercises.insert(actualTarget, itemToMove);
                endMoveRows(); // Ilmoitetaan siirron valmistumisesta
            }
        }

}

void ExerciseListModel::copyItems(QList<int> selectedIndices, int targetIndex)
{
    if (selectedIndices.isEmpty() || targetIndex < 0 || targetIndex > mExercises.count())
            return;

        // 1. Lajitellaan valitut indeksit SUURIMMASTA PIENIMPÄÄN
        std::sort(selectedIndices.begin(), selectedIndices.end(), std::greater<int>());

        // 2. Suoritetaan kopioinnit yksitellen
        for (int sourceIdx : selectedIndices) {
            if (sourceIdx < 0 || sourceIdx >= mExercises.count())
                continue;

            // Ilmoitetaan QML-näkymälle, että uusi rivi lisätään kohdeindeksiin
            beginInsertRows(QModelIndex(), targetIndex, targetIndex);

            auto originalItem = mExercises.at(sourceIdx);
            auto copiedItem = originalItem->clone();
            mExercises.insert(targetIndex, copiedItem);

            endInsertRows(); // Ilmoitetaan lisäyksen valmistumisesta
        }
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
