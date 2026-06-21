#ifndef TST_EXERCISESET_H
#define TST_EXERCISESET_H

#include <QObject>

class TstExerciseSet : public QObject
{
    Q_OBJECT

private slots:
    void roundsDefaultsToOne();
    void setRoundsClampsBelowOne();
    void appendIncreasesCount();
    void removeDecreasesCount();
    void emptySetIsInvalid();
    void setWithInvalidExerciseIsInvalid();
    void setWithAllValidExercisesIsValid();
    void totalDurationSecondsAccountsForExerciseRounds();
    void insertAtPosition();
    void moveItemsSingleDown();
    void copyItemsInsertsClones();
    void clone();
    void totalDurationChangedEmitsAbsoluteTotalOnAppend();
    void totalDurationChangedEmitsAbsoluteTotalOnRemove();
    void totalDurationChangedPropagatesFromChildDurationChange();
    void totalDurationChangedPropagatesFromChildRoundsChange();
    void validityChangedOnAppendAndRemove();
    void validityChangedPropagatesFromChild();
    void copiedExerciseDurationChangePropagates();
};

#endif // TST_EXERCISESET_H
