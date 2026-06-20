#include "tst_exercisetimer.h"

#include <QtTest>
#include <QSettings>
#include "exercisetimer.h"
#include "exerciselistmodel.h"
#include "timedexercise.h"

void TstExerciseTimer::init()
{
    // ExerciseTimer reads/writes "start delay" and "end notification time"
    // via QSettings. Clear them before each test so defaults are
    // deterministic and tests don't leak settings into one another.
    // tests/main.cpp sets a dedicated, test-mode QSettings namespace.
    QSettings().clear();
}

void TstExerciseTimer::durationAggregation()
{
    ExerciseTimer timer(nullptr, false);
    timer.addExercise(new TimedExercise("work", 1, 0, 0));  // 60 s
    timer.addExercise(new TimedExercise("rest", 0, 30, 0)); // 30 s

    QCOMPARE(timer.totalDuration(), QTime(0, 1, 30));

    timer.removeExercise(0);

    QCOMPARE(timer.totalDuration(), QTime(0, 0, 30));
}

void TstExerciseTimer::validityReflectsExercises()
{
    ExerciseTimer timer(nullptr, false);
    QVERIFY(!timer.allExercisesValid());

    timer.addExercise(new TimedExercise("work", 1, 0, 0)); // valid
    QVERIFY(timer.allExercisesValid());

    timer.addExercise(new TimedExercise("work", 0, 0, 0)); // invalid (zero duration)
    QVERIFY(!timer.allExercisesValid());

    timer.removeExercise(1); // remove the invalid one
    QVERIFY(timer.allExercisesValid());
}

void TstExerciseTimer::startDelayBounds()
{
    ExerciseTimer timer(nullptr, false);
    QCOMPARE(timer.startDelay(), 5);

    QSignalSpy spy(&timer, &ExerciseTimer::startDelayChanged);
    timer.setStartDelay(10);
    QCOMPARE(timer.startDelay(), 10);
    QCOMPARE(spy.count(), 1);

    timer.setStartDelay(-1);
    QCOMPARE(timer.startDelay(), 10);
    timer.setStartDelay(60);
    QCOMPARE(timer.startDelay(), 10);
    QCOMPARE(spy.count(), 1);
}

void TstExerciseTimer::startDelayPersists()
{
    ExerciseTimer timer(nullptr, false);
    timer.setStartDelay(15);

    ExerciseTimer freshTimer(nullptr, false);
    QCOMPARE(freshTimer.startDelay(), 15);
}

void TstExerciseTimer::endWarningTimeBounds()
{
    ExerciseTimer timer(nullptr, false);
    QCOMPARE(timer.endWarningTime(), 3);

    QSignalSpy spy(&timer, &ExerciseTimer::endWarningTimeChanged);
    timer.setEndWarningTime(8);
    QCOMPARE(timer.endWarningTime(), 8);
    QCOMPARE(spy.count(), 1);

    timer.setEndWarningTime(-1);
    QCOMPARE(timer.endWarningTime(), 8);
    timer.setEndWarningTime(60);
    QCOMPARE(timer.endWarningTime(), 8);
    QCOMPARE(spy.count(), 1);
}

void TstExerciseTimer::endWarningTimePersists()
{
    ExerciseTimer timer(nullptr, false);
    timer.setEndWarningTime(12);

    ExerciseTimer freshTimer(nullptr, false);
    QCOMPARE(freshTimer.endWarningTime(), 12);
}

void TstExerciseTimer::resetOnFreshTimer()
{
    ExerciseTimer timer(nullptr, false);
    timer.addExercise(new TimedExercise("work", 1, 0, 0));
    timer.addExercise(new TimedExercise("rest", 0, 30, 0));

    QSignalSpy activitySpy(&timer, &ExerciseTimer::currentActivityChanged);
    QSignalSpy durationSpy(&timer, &ExerciseTimer::currentDurationChanged);
    QSignalSpy currentRunningSpy(&timer, &ExerciseTimer::currentRunningTimeChanged);
    QSignalSpy totalRunningSpy(&timer, &ExerciseTimer::totalRunningTimeChanged);

    timer.reset();

    QCOMPARE(timer.currentRunningTime(), QTime(0, 0, 0));
    QCOMPARE(timer.totalRunningTime(), QTime(0, 0, 0));
    QCOMPARE(timer.currentActivity(), timer.exerciseListModel()->at(0));
    QCOMPARE(activitySpy.count(), 1);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(currentRunningSpy.count(), 1);
    QCOMPARE(totalRunningSpy.count(), 1);
}

void TstExerciseTimer::currentActivityOnEmptyModel()
{
    ExerciseTimer timer(nullptr, false);
    QCOMPARE(timer.currentActivity(), static_cast<TimedExercise *>(nullptr));

    timer.addExercise(new TimedExercise("work", 1, 0, 0));
    QCOMPARE(timer.currentActivity(), timer.exerciseListModel()->at(0));
}

void TstExerciseTimer::startOnEmptyModelIsNoOp()
{
    ExerciseTimer timer(nullptr, false);
    QVERIFY(!timer.running());

    timer.start();

    QVERIFY(!timer.running());
}

void TstExerciseTimer::startThenPauseTransitionsRunning()
{
    ExerciseTimer timer(nullptr, false);
    timer.setStartDelay(0); // start immediately, no countdown
    timer.addExercise(new TimedExercise("work", 0, 1, 0));

    QSignalSpy runningSpy(&timer, &ExerciseTimer::runningStatusChanged);

    QVERIFY(!timer.running());
    timer.start();
    QVERIFY(timer.running());
    timer.pause();
    QVERIFY(!timer.running());

    QCOMPARE(runningSpy.count(), 2);
    QCOMPARE(runningSpy.at(0).at(0).toBool(), true);
    QCOMPARE(runningSpy.at(1).at(0).toBool(), false);
}
