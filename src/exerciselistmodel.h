#pragma once

#include <QAbstractListModel>
#include <QTime>

class TimedExercise;

class ExerciseListModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum Roles {ObjectRole = Qt::UserRole + 1};

    explicit ExerciseListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = ObjectRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void appendExercise(TimedExercise *exercise);
    void insertExercise(TimedExercise *exercise, int position);
    Q_INVOKABLE void removeExercise(int index);
    inline bool isEmpty() const { return mExercises.isEmpty(); }
    inline TimedExercise *at(int i) const { return mExercises.at(i); }
    inline int size() const { return mExercises.size(); }
    //bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;
    //flags()

signals:
    void totalDurationChanged(int durationChangeSeconds);

private slots:
    void exerciseDurationChanged(int durationChangeSeconds);

private:
   int mTotalDurationSeconds;
   QVector<TimedExercise*> mExercises;

};

