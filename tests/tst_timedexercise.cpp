#include "tst_timedexercise.h"

#include <QtTest>
#include "timedexercise.h"

void TstTimedExercise::defaults()
{
    TimedExercise ex;
    QCOMPARE(ex.activityType(), QString("work"));
    QCOMPARE(ex.mins(), 1);
    QCOMPARE(ex.secs(), 0);
    QCOMPARE(ex.reps(), 0);
}

void TstTimedExercise::duration()
{
    TimedExercise ex("work", 2, 30, 0);
    QCOMPARE(ex.duration(), QTime(0, 2, 30));
}

void TstTimedExercise::durationSeconds_data()
{
    QTest::addColumn<int>("mins");
    QTest::addColumn<int>("secs");
    QTest::addColumn<int>("expected");

    QTest::newRow("zero") << 0 << 0 << 0;
    QTest::newRow("secs only") << 0 << 45 << 45;
    QTest::newRow("mins only") << 2 << 0 << 120;
    QTest::newRow("mins and secs") << 1 << 30 << 90;
}

void TstTimedExercise::durationSeconds()
{
    QFETCH(int, mins);
    QFETCH(int, secs);
    QFETCH(int, expected);

    TimedExercise ex("work", mins, secs, 0);
    QCOMPARE(ex.durationSeconds(), expected);
}

void TstTimedExercise::isValid_data()
{
    QTest::addColumn<int>("mins");
    QTest::addColumn<int>("secs");
    QTest::addColumn<bool>("expected");

    QTest::newRow("zero duration") << 0 << 0 << false;
    QTest::newRow("seconds only") << 0 << 1 << true;
    QTest::newRow("minutes only") << 1 << 0 << true;
}

void TstTimedExercise::isValid()
{
    QFETCH(int, mins);
    QFETCH(int, secs);
    QFETCH(bool, expected);

    TimedExercise ex("work", mins, secs, 0);
    QCOMPARE(ex.isValid(), expected);
}

void TstTimedExercise::rpm_data()
{
    QTest::addColumn<int>("mins");
    QTest::addColumn<int>("secs");
    QTest::addColumn<int>("reps");
    QTest::addColumn<double>("expectedRpm");
    QTest::addColumn<double>("expectedSeparation");

    QTest::newRow("1 min, 10 reps") << 1 << 0 << 10 << 10.0 << 6.0;
    QTest::newRow("no reps") << 1 << 0 << 0 << 0.0 << 0.0;
    QTest::newRow("zero duration") << 0 << 0 << 5 << 0.0 << 0.0;
}

void TstTimedExercise::rpm()
{
    QFETCH(int, mins);
    QFETCH(int, secs);
    QFETCH(int, reps);
    QFETCH(double, expectedRpm);
    QFETCH(double, expectedSeparation);

    TimedExercise ex("work", mins, secs, reps);
    QCOMPARE(ex.rpm(), expectedRpm);
    QCOMPARE(ex.repSeparation(), expectedSeparation);
}

void TstTimedExercise::setMinsEmitsOnChange()
{
    TimedExercise ex("work", 1, 0, 0);
    QSignalSpy minsSpy(&ex, &TimedExercise::minsChanged);
    QSignalSpy durationSpy(&ex, &TimedExercise::durationChanged);

    ex.setMins(2);

    QCOMPARE(ex.mins(), 2);
    QCOMPARE(minsSpy.count(), 1);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 60);
}

void TstTimedExercise::setMinsNoOpDoesNotEmit()
{
    TimedExercise ex("work", 1, 0, 0);
    QSignalSpy minsSpy(&ex, &TimedExercise::minsChanged);

    ex.setMins(1);

    QCOMPARE(minsSpy.count(), 0);
}

void TstTimedExercise::setSecsEmitsDurationDelta()
{
    TimedExercise ex("work", 1, 10, 0);
    QSignalSpy durationSpy(&ex, &TimedExercise::durationChanged);

    ex.setSecs(5);

    QCOMPARE(ex.secs(), 5);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), -5);
}

void TstTimedExercise::setRepsEmitsOnChange()
{
    TimedExercise ex("work", 1, 0, 0);
    QSignalSpy repsSpy(&ex, &TimedExercise::repsChanged);

    ex.setReps(8);

    QCOMPARE(ex.reps(), 8);
    QCOMPARE(repsSpy.count(), 1);
}

void TstTimedExercise::setActivityTypeEmitsOnChange()
{
    TimedExercise ex("work", 1, 0, 0);
    QSignalSpy typeSpy(&ex, &TimedExercise::activityTypeChanged);

    ex.setActivityType("rest");
    QCOMPARE(ex.activityType(), QString("rest"));
    QCOMPARE(typeSpy.count(), 1);

    ex.setActivityType("rest");
    QCOMPARE(typeSpy.count(), 1);
}

void TstTimedExercise::toggleActivityType()
{
    TimedExercise ex("work", 1, 0, 0);

    ex.toggleActivityType();
    QCOMPARE(ex.activityType(), QString("rest"));

    ex.toggleActivityType();
    QCOMPARE(ex.activityType(), QString("work"));
}

void TstTimedExercise::clone()
{
    TimedExercise ex("rest", 3, 15, 7);
    ex.setRounds(4);
    TimedExercise *copy = ex.clone();

    QCOMPARE(copy->activityType(), ex.activityType());
    QCOMPARE(copy->mins(), ex.mins());
    QCOMPARE(copy->secs(), ex.secs());
    QCOMPARE(copy->reps(), ex.reps());
    QCOMPARE(copy->rounds(), ex.rounds());
    QVERIFY(copy != &ex);

    delete copy;
}

void TstTimedExercise::roundsDefaultsToOne()
{
    TimedExercise ex;
    QCOMPARE(ex.rounds(), 1);
}

void TstTimedExercise::setRoundsEmitsOnChange()
{
    TimedExercise ex("work", 1, 0, 0);
    QSignalSpy roundsSpy(&ex, &TimedExercise::roundsChanged);

    ex.setRounds(3);

    QCOMPARE(ex.rounds(), 3);
    QCOMPARE(roundsSpy.count(), 1);

    ex.setRounds(3);
    QCOMPARE(roundsSpy.count(), 1);
}

void TstTimedExercise::setRoundsClampsBelowOne()
{
    TimedExercise ex("work", 1, 0, 0);

    ex.setRounds(0);
    QCOMPARE(ex.rounds(), 1);

    ex.setRounds(-5);
    QCOMPARE(ex.rounds(), 1);
}
