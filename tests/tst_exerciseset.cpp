#include "tst_exerciseset.h"

#include <QtTest>
#include "exerciseset.h"
#include "timedexercise.h"

void TstExerciseSet::roundsDefaultsToOne()
{
    ExerciseSet set;
    QCOMPARE(set.rounds(), 1);
}

void TstExerciseSet::setRoundsClampsBelowOne()
{
    ExerciseSet set;

    set.setRounds(0);
    QCOMPARE(set.rounds(), 1);

    set.setRounds(-3);
    QCOMPARE(set.rounds(), 1);

    set.setRounds(4);
    QCOMPARE(set.rounds(), 4);
}

void TstExerciseSet::appendIncreasesCount()
{
    ExerciseSet set;
    set.appendExercise(new TimedExercise("work", 1, 0, 0));
    set.appendExercise(new TimedExercise("rest", 0, 30, 0));

    QCOMPARE(set.count(), 2);
    QVERIFY(!set.isEmpty());
    QCOMPARE(set.at(0)->activityType(), QString("work"));
    QCOMPARE(set.at(1)->activityType(), QString("rest"));
}

void TstExerciseSet::removeDecreasesCount()
{
    ExerciseSet set;
    set.appendExercise(new TimedExercise("work", 1, 0, 0));
    set.appendExercise(new TimedExercise("work", 2, 0, 0));

    set.removeExercise(0);

    QCOMPARE(set.count(), 1);
    QCOMPARE(set.at(0)->mins(), 2);
}

void TstExerciseSet::emptySetIsInvalid()
{
    ExerciseSet set;
    QVERIFY(!set.isValid());
}

void TstExerciseSet::setWithInvalidExerciseIsInvalid()
{
    ExerciseSet set;
    set.appendExercise(new TimedExercise("work", 1, 0, 0));   // valid
    set.appendExercise(new TimedExercise("work", 0, 0, 0));   // invalid: zero duration

    QVERIFY(!set.isValid());
}

void TstExerciseSet::setWithAllValidExercisesIsValid()
{
    ExerciseSet set;
    set.appendExercise(new TimedExercise("work", 1, 0, 0));
    set.appendExercise(new TimedExercise("rest", 0, 30, 0));

    QVERIFY(set.isValid());
}

void TstExerciseSet::totalDurationSecondsAccountsForExerciseRounds()
{
    ExerciseSet set;
    TimedExercise *a = new TimedExercise("work", 1, 0, 0); // 60 s
    a->setRounds(3);
    TimedExercise *b = new TimedExercise("rest", 0, 30, 0); // 30 s, 1 round
    set.appendExercise(a);
    set.appendExercise(b);

    QCOMPARE(set.totalDurationSeconds(), 60 * 3 + 30);
}

void TstExerciseSet::insertAtPosition()
{
    ExerciseSet set;
    TimedExercise *a = new TimedExercise("work", 1, 0, 0);
    TimedExercise *b = new TimedExercise("work", 2, 0, 0);
    set.appendExercise(a);
    set.insertExercise(b, 0);

    QCOMPARE(set.count(), 2);
    QCOMPARE(set.at(0), b);
    QCOMPARE(set.at(1), a);
}

void TstExerciseSet::moveItemsSingleDown()
{
    ExerciseSet set;
    TimedExercise *a = new TimedExercise("work", 1, 0, 0);
    TimedExercise *b = new TimedExercise("work", 2, 0, 0);
    TimedExercise *c = new TimedExercise("work", 3, 0, 0);
    set.appendExercise(a);
    set.appendExercise(b);
    set.appendExercise(c);

    set.moveItems(QList<int>{0}, 2);

    QCOMPARE(set.count(), 3);
    QCOMPARE(set.at(0), b);
    QCOMPARE(set.at(1), a);
    QCOMPARE(set.at(2), c);
}

void TstExerciseSet::copyItemsInsertsClones()
{
    ExerciseSet set;
    TimedExercise *a = new TimedExercise("work", 1, 0, 0);
    TimedExercise *b = new TimedExercise("work", 2, 0, 0);
    set.appendExercise(a);
    set.appendExercise(b);

    set.copyItems(QList<int>{0}, 1);

    QCOMPARE(set.count(), 3);
    QCOMPARE(set.at(0), a);
    QVERIFY(set.at(1) != a);
    QCOMPARE(set.at(1)->durationSeconds(), a->durationSeconds());
    QCOMPARE(set.at(2), b);
}

void TstExerciseSet::clone()
{
    ExerciseSet set;
    set.setRounds(3);
    set.appendExercise(new TimedExercise("work", 1, 0, 0));
    set.appendExercise(new TimedExercise("rest", 0, 30, 0));

    ExerciseSet *copy = set.clone();

    QCOMPARE(copy->rounds(), 3);
    QCOMPARE(copy->count(), 2);
    QVERIFY(copy->at(0) != set.at(0));
    QCOMPARE(copy->at(0)->activityType(), set.at(0)->activityType());
    QCOMPARE(copy->at(1)->durationSeconds(), set.at(1)->durationSeconds());

    delete copy;
}

void TstExerciseSet::totalDurationChangedEmitsAbsoluteTotalOnAppend()
{
    ExerciseSet set;
    QSignalSpy durationSpy(&set, &ExerciseSet::totalDurationChanged);

    set.appendExercise(new TimedExercise("work", 1, 0, 0)); // 60 s
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 60);

    set.appendExercise(new TimedExercise("rest", 0, 30, 0)); // 30 s
    QCOMPARE(durationSpy.count(), 2);
    QCOMPARE(durationSpy.at(1).at(0).toInt(), 90);
}

void TstExerciseSet::totalDurationChangedEmitsAbsoluteTotalOnRemove()
{
    ExerciseSet set;
    set.appendExercise(new TimedExercise("work", 1, 0, 0));  // 60 s
    set.appendExercise(new TimedExercise("rest", 0, 30, 0)); // 30 s

    QSignalSpy durationSpy(&set, &ExerciseSet::totalDurationChanged);
    set.removeExercise(0);

    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 30);
}

void TstExerciseSet::totalDurationChangedPropagatesFromChildDurationChange()
{
    ExerciseSet set;
    TimedExercise *ex = new TimedExercise("work", 1, 0, 0); // 60 s
    set.appendExercise(ex);

    QSignalSpy durationSpy(&set, &ExerciseSet::totalDurationChanged);
    ex->setMins(2); // 120 s

    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 120);
}

void TstExerciseSet::totalDurationChangedPropagatesFromChildRoundsChange()
{
    ExerciseSet set;
    TimedExercise *ex = new TimedExercise("work", 1, 0, 0); // 60 s
    set.appendExercise(ex);

    QSignalSpy durationSpy(&set, &ExerciseSet::totalDurationChanged);
    ex->setRounds(3); // 180 s

    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 180);
}

void TstExerciseSet::validityChangedOnAppendAndRemove()
{
    ExerciseSet set;
    QSignalSpy validitySpy(&set, &ExerciseSet::validityChanged);

    set.appendExercise(new TimedExercise("work", 1, 0, 0)); // now valid (was empty/invalid)
    QCOMPARE(validitySpy.count(), 1);
    QCOMPARE(validitySpy.at(0).at(0).toBool(), true);

    set.removeExercise(0); // now empty/invalid again
    QCOMPARE(validitySpy.count(), 2);
    QCOMPARE(validitySpy.at(1).at(0).toBool(), false);
}

void TstExerciseSet::validityChangedPropagatesFromChild()
{
    ExerciseSet set;
    TimedExercise *ex = new TimedExercise("work", 1, 0, 0);
    set.appendExercise(ex);

    QSignalSpy validitySpy(&set, &ExerciseSet::validityChanged);
    ex->setMins(0); // now invalid: zero duration
    ex->setSecs(0);

    QVERIFY(validitySpy.count() >= 1);
    QCOMPARE(validitySpy.at(validitySpy.count() - 1).at(0).toBool(), false);
}

void TstExerciseSet::copiedExerciseDurationChangePropagates()
{
    ExerciseSet set;
    set.appendExercise(new TimedExercise("work", 1, 0, 0)); // 60 s

    set.copyItems(QList<int>{0}, 1);
    QCOMPARE(set.count(), 2);

    QSignalSpy durationSpy(&set, &ExerciseSet::totalDurationChanged);
    set.at(1)->setMins(2); // copy's duration changes: 60 + 120 = 180 s

    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 180);
}

void TstExerciseSet::countChangedOnAppendAndRemove()
{
    ExerciseSet set;
    QSignalSpy countSpy(&set, &ExerciseSet::countChanged);

    set.appendExercise(new TimedExercise("work", 1, 0, 0));
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(set.count(), 1);

    set.removeExercise(0);
    QCOMPARE(countSpy.count(), 2);
    QCOMPARE(set.count(), 0);
}
