#pragma once

#include <QAbstractListModel>
#include <QTime>

class TimedExercise;

class ExerciseListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:

    enum Roles {ObjectRole = Qt::UserRole + 1};

    explicit ExerciseListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = ObjectRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void appendExercise(TimedExercise *exercise);
    void insertExercise(TimedExercise *exercise, int position);
    Q_INVOKABLE void removeExercise(int index);
    Q_INVOKABLE void clear();
    inline bool isEmpty() const { return mExercises.isEmpty(); }
    inline TimedExercise *at(int i) const { return mExercises.at(i); }
    inline int size() const { return mExercises.size(); }
    Q_INVOKABLE int count() const {return mExercises.count(); }
    //bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;
    //flags()

signals:
    void totalDurationChanged(int durationChangeSeconds);
    void countChanged();

private slots:
    void exerciseDurationChanged(int durationChangeSeconds);

private:
   int mTotalDurationSeconds;
   QVector<TimedExercise*> mExercises;

};

