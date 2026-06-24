#ifndef TST_EXERCISETIMER_H
#define TST_EXERCISETIMER_H

#include <QObject>

class TstExerciseTimer : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void durationAggregation();
    void validityReflectsExercises();
    void startDelayBounds();
    void startDelayPersists();
    void endWarningTimeBounds();
    void endWarningTimePersists();
    void muteSoundsToggleAndPersists();
    void resetOnFreshTimer();
    void currentActivityOnEmptyModel();
    void startOnEmptyModelIsNoOp();
    void startThenPauseTransitionsRunning();

    void emptySetBlocksValidity();
    void noSetsBlocksValidity();
    void durationAccountsForSetAndExerciseRounds();
    void playSequenceFlattensSetAndExerciseRounds();
    void positionPropertiesDuringPlayback();
    void outOfRangeSetIndexIsNoOp();
};

#endif // TST_EXERCISETIMER_H
