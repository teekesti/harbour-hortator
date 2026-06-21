#include "tst_exerciselistmodel.h"

#include <QtTest>
#include "exerciselistmodel.h"
#include "exerciseset.h"
#include "timedexercise.h"

namespace {
ExerciseSet *setWithOneExercise(QString activityType, int mins, int secs)
{
    ExerciseSet *set = new ExerciseSet();
    set->appendExercise(new TimedExercise(activityType, mins, secs, 0));
    return set;
}
}

void TstExerciseListModel::appendIncreasesCount()
{
    ExerciseListModel model;
    model.appendSet(setWithOneExercise("work", 1, 0));
    model.appendSet(setWithOneExercise("rest", 0, 30));

    QCOMPARE(model.count(), 2);
    QCOMPARE(model.rowCount(), 2);
    QVERIFY(!model.isEmpty());
}

void TstExerciseListModel::insertAtPosition()
{
    ExerciseListModel model;
    ExerciseSet *a = setWithOneExercise("work", 1, 0);
    ExerciseSet *b = setWithOneExercise("work", 2, 0);
    model.appendSet(a);
    model.insertSet(b, 0);

    QCOMPARE(model.count(), 2);
    QCOMPARE(model.at(0), b);
    QCOMPARE(model.at(1), a);
}

void TstExerciseListModel::insertOutOfRangeIsNoOp()
{
    ExerciseListModel model;
    model.appendSet(setWithOneExercise("work", 1, 0));

    model.insertSet(setWithOneExercise("work", 2, 0), -1);
    model.insertSet(setWithOneExercise("work", 3, 0), 5);

    QCOMPARE(model.count(), 1);
}

void TstExerciseListModel::dataReturnsExercisePointer()
{
    ExerciseListModel model;
    ExerciseSet *set = setWithOneExercise("work", 1, 0);
    model.appendSet(set);

    QVariant value = model.data(model.index(0, 0), ExerciseListModel::SetRole);
    QCOMPARE(value.value<ExerciseSet *>(), set);
}

void TstExerciseListModel::roleNamesMapping()
{
    ExerciseListModel model;
    QHash<int, QByteArray> roles = model.roleNames();

    QCOMPARE(roles.value(ExerciseListModel::SetRole), QByteArray("set"));
}

void TstExerciseListModel::totalDurationChangedOnInsert()
{
    ExerciseListModel model;
    QSignalSpy durationSpy(&model, &ExerciseListModel::totalDurationChanged);

    model.appendSet(setWithOneExercise("work", 1, 30)); // 90 s

    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 90);
}

void TstExerciseListModel::removeDecreasesCountAndDuration()
{
    ExerciseListModel model;
    model.appendSet(setWithOneExercise("work", 1, 0)); // 60 s
    model.appendSet(setWithOneExercise("work", 2, 0)); // 120 s

    QSignalSpy durationSpy(&model, &ExerciseListModel::totalDurationChanged);
    QSignalSpy countSpy(&model, &ExerciseListModel::countChanged);

    model.removeSet(0);

    QCOMPARE(model.count(), 1);
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 120);
}

void TstExerciseListModel::removeOutOfRangeIsNoOp()
{
    ExerciseListModel model;
    model.appendSet(setWithOneExercise("work", 1, 0));

    model.removeSet(-1);
    model.removeSet(5);

    QCOMPARE(model.count(), 1);
}

void TstExerciseListModel::clearEmptiesModel()
{
    ExerciseListModel model;
    model.appendSet(setWithOneExercise("work", 1, 0)); // 60 s
    model.appendSet(setWithOneExercise("work", 2, 0)); // 120 s

    QSignalSpy durationSpy(&model, &ExerciseListModel::totalDurationChanged);
    QSignalSpy countSpy(&model, &ExerciseListModel::countChanged);

    model.clear();

    QCOMPARE(model.count(), 0);
    QVERIFY(model.isEmpty());
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 0);
}

void TstExerciseListModel::clearOnEmptyIsNoOp()
{
    ExerciseListModel model;
    QSignalSpy countSpy(&model, &ExerciseListModel::countChanged);

    model.clear();

    QCOMPARE(model.count(), 0);
    QCOMPARE(countSpy.count(), 0);
}

void TstExerciseListModel::exerciseDurationChangePropagates()
{
    ExerciseListModel model;
    ExerciseSet *set = setWithOneExercise("work", 1, 0); // 60 s
    model.appendSet(set);

    QSignalSpy durationSpy(&model, &ExerciseListModel::totalDurationChanged);

    set->at(0)->setMins(2); // 120 s

    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 120);
}

void TstExerciseListModel::setRoundsChangePropagates()
{
    ExerciseListModel model;
    ExerciseSet *set = setWithOneExercise("work", 1, 0); // 60 s
    model.appendSet(set);

    QSignalSpy durationSpy(&model, &ExerciseListModel::totalDurationChanged);

    set->setRounds(3); // 180 s

    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 180);
}

void TstExerciseListModel::moveItemsSingleDown()
{
    ExerciseListModel model;
    ExerciseSet *a = setWithOneExercise("work", 1, 0);
    ExerciseSet *b = setWithOneExercise("work", 2, 0);
    ExerciseSet *c = setWithOneExercise("work", 3, 0);
    ExerciseSet *d = setWithOneExercise("work", 4, 0);
    model.appendSet(a);
    model.appendSet(b);
    model.appendSet(c);
    model.appendSet(d);

    // Move B (index 1) to target index 3.
    model.moveItems(QList<int>{1}, 3);

    QCOMPARE(model.count(), 4);
    QCOMPARE(model.at(0), a);
    QCOMPARE(model.at(1), c);
    QCOMPARE(model.at(2), b);
    QCOMPARE(model.at(3), d);
}

void TstExerciseListModel::moveItemsSingleUp()
{
    ExerciseListModel model;
    ExerciseSet *a = setWithOneExercise("work", 1, 0);
    ExerciseSet *b = setWithOneExercise("work", 2, 0);
    ExerciseSet *c = setWithOneExercise("work", 3, 0);
    ExerciseSet *d = setWithOneExercise("work", 4, 0);
    model.appendSet(a);
    model.appendSet(b);
    model.appendSet(c);
    model.appendSet(d);

    // Move D (index 3) to target index 0.
    model.moveItems(QList<int>{3}, 0);

    QCOMPARE(model.count(), 4);
    QCOMPARE(model.at(0), d);
    QCOMPARE(model.at(1), a);
    QCOMPARE(model.at(2), b);
    QCOMPARE(model.at(3), c);
}

void TstExerciseListModel::moveItemsMultiSelect()
{
    ExerciseListModel model;
    ExerciseSet *a = setWithOneExercise("work", 1, 0);
    ExerciseSet *b = setWithOneExercise("work", 2, 0);
    ExerciseSet *c = setWithOneExercise("work", 3, 0);
    ExerciseSet *d = setWithOneExercise("work", 4, 0);
    ExerciseSet *e = setWithOneExercise("work", 5, 0);
    model.appendSet(a);
    model.appendSet(b);
    model.appendSet(c);
    model.appendSet(d);
    model.appendSet(e);

    // Move A (index 0) and C (index 2) toward target index 4.
    model.moveItems(QList<int>{0, 2}, 4);

    QCOMPARE(model.count(), 5);
    QCOMPARE(model.at(0), b);
    QCOMPARE(model.at(1), d);
    QCOMPARE(model.at(2), c);
    QCOMPARE(model.at(3), a);
    QCOMPARE(model.at(4), e);
}

void TstExerciseListModel::copyItemsInsertsClones()
{
    ExerciseListModel model;
    ExerciseSet *a = setWithOneExercise("work", 1, 0);
    ExerciseSet *b = setWithOneExercise("work", 2, 0);
    ExerciseSet *c = setWithOneExercise("work", 3, 0);
    model.appendSet(a);
    model.appendSet(b);
    model.appendSet(c);

    // Copy A (index 0) and C (index 2) to target index 1.
    model.copyItems(QList<int>{0, 2}, 1);

    QCOMPARE(model.count(), 5);
    QCOMPARE(model.at(0), a);
    QVERIFY(model.at(1) != a);
    QCOMPARE(model.at(1)->totalDurationSeconds(), a->totalDurationSeconds());
    QVERIFY(model.at(2) != c);
    QCOMPARE(model.at(2)->totalDurationSeconds(), c->totalDurationSeconds());
    QCOMPARE(model.at(3), b);
    QCOMPARE(model.at(4), c);
}
