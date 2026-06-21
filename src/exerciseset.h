#ifndef EXERCISESET_H
#define EXERCISESET_H

#include <QObject>
#include <QVector>
#include <QList>

class TimedExercise;

/*! An ordered group of TimedExercise entries that is itself repeated as a
whole (its rounds count) before the workout moves on to the next set. */
class ExerciseSet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int rounds READ rounds WRITE setRounds NOTIFY roundsChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY validityChanged)
public:
    explicit ExerciseSet(QObject *parent = 0);
    int rounds() const;
    int count() const;
    bool isEmpty() const;
    Q_INVOKABLE TimedExercise *at(int i) const;
    bool isValid() const;
    int totalDurationSeconds() const;
    ExerciseSet *clone() const;
    void appendExercise(TimedExercise *exercise);
    void insertExercise(TimedExercise *exercise, int position);
    void removeExercise(int index);
    void moveItems(QList<int> selectedIndices, int targetIndex);
    void copyItems(QList<int> selectedIndices, int targetIndex);

signals:
    void roundsChanged(int rounds);
    void countChanged();
    /*! Emitted with the new absolute total (not a delta) whenever this
    set's totalDurationSeconds() changes, for any reason: an exercise is
    added/removed, or a child exercise's duration or rounds changes. */
    void totalDurationChanged(int newTotalDurationSeconds);
    /*! Emitted with the new absolute validity whenever isValid() changes:
    an exercise is added/removed, or a child exercise's own validity
    changes. */
    void validityChanged(bool isValid);

public slots:
    void setRounds(int rounds);

private slots:
    void onChildDurationOrRoundsChanged();
    void onChildValidityChanged();

private: // data
    int mRounds;
    bool mWasValid;
    QVector<TimedExercise *> mExercises;
};

#endif // EXERCISESET_H
