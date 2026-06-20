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
    void resetOnFreshTimer();
    void currentActivityOnEmptyModel();
    void startOnEmptyModelIsNoOp();
    void startThenPauseTransitionsRunning();
};

#endif // TST_EXERCISETIMER_H
