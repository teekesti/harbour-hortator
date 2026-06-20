#include "tst_exerciselistmodel.h"

#include <QtTest>
#include "exerciselistmodel.h"
#include "timedexercise.h"

void TstExerciseListModel::appendIncreasesCount()
{
    ExerciseListModel model;
    model.appendExercise(new TimedExercise("work", 1, 0, 0));
    model.appendExercise(new TimedExercise("rest", 0, 30, 0));

    QCOMPARE(model.count(), 2);
    QCOMPARE(model.rowCount(), 2);
    QVERIFY(!model.isEmpty());
}

void TstExerciseListModel::insertAtPosition()
{
    ExerciseListModel model;
    TimedExercise *a = new TimedExercise("work", 1, 0, 0);
    TimedExercise *b = new TimedExercise("work", 2, 0, 0);
    model.appendExercise(a);
    model.insertExercise(b, 0);

    QCOMPARE(model.count(), 2);
    QCOMPARE(model.at(0), b);
    QCOMPARE(model.at(1), a);
}

void TstExerciseListModel::dataReturnsExercisePointer()
{
    ExerciseListModel model;
    TimedExercise *ex = new TimedExercise("work", 1, 0, 0);
    model.appendExercise(ex);

    QVariant value = model.data(model.index(0, 0), ExerciseListModel::ExerciseRole);
    QCOMPARE(value.value<TimedExercise *>(), ex);
}

void TstExerciseListModel::roleNamesMapping()
{
    ExerciseListModel model;
    QHash<int, QByteArray> roles = model.roleNames();

    QCOMPARE(roles.value(ExerciseListModel::ExerciseRole), QByteArray("exercise"));
}

void TstExerciseListModel::totalDurationChangedOnInsert()
{
    ExerciseListModel model;
    QSignalSpy durationSpy(&model, &ExerciseListModel::totalDurationChanged);

    model.appendExercise(new TimedExercise("work", 1, 30, 0));

    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 90);
}

void TstExerciseListModel::removeDecreasesCountAndDuration()
{
    ExerciseListModel model;
    model.appendExercise(new TimedExercise("work", 1, 0, 0));
    model.appendExercise(new TimedExercise("work", 2, 0, 0));

    QSignalSpy durationSpy(&model, &ExerciseListModel::totalDurationChanged);
    QSignalSpy countSpy(&model, &ExerciseListModel::countChanged);

    model.removeExercise(0);

    QCOMPARE(model.count(), 1);
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), -60);
}

void TstExerciseListModel::removeOutOfRangeIsNoOp()
{
    ExerciseListModel model;
    model.appendExercise(new TimedExercise("work", 1, 0, 0));

    model.removeExercise(-1);
    model.removeExercise(5);

    QCOMPARE(model.count(), 1);
}

void TstExerciseListModel::clearEmptiesModel()
{
    ExerciseListModel model;
    model.appendExercise(new TimedExercise("work", 1, 0, 0));
    model.appendExercise(new TimedExercise("work", 2, 0, 0));

    QSignalSpy durationSpy(&model, &ExerciseListModel::totalDurationChanged);
    QSignalSpy countSpy(&model, &ExerciseListModel::countChanged);

    model.clear();

    QCOMPARE(model.count(), 0);
    QVERIFY(model.isEmpty());
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), -180);
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
    TimedExercise *ex = new TimedExercise("work", 1, 0, 0);
    model.appendExercise(ex);

    QSignalSpy durationSpy(&model, &ExerciseListModel::totalDurationChanged);

    ex->setMins(2);

    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(durationSpy.at(0).at(0).toInt(), 60);
}

void TstExerciseListModel::moveItemsSingleDown()
{
    ExerciseListModel model;
    TimedExercise *a = new TimedExercise("work", 1, 0, 0);
    TimedExercise *b = new TimedExercise("work", 2, 0, 0);
    TimedExercise *c = new TimedExercise("work", 3, 0, 0);
    TimedExercise *d = new TimedExercise("work", 4, 0, 0);
    model.appendExercise(a);
    model.appendExercise(b);
    model.appendExercise(c);
    model.appendExercise(d);

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
    TimedExercise *a = new TimedExercise("work", 1, 0, 0);
    TimedExercise *b = new TimedExercise("work", 2, 0, 0);
    TimedExercise *c = new TimedExercise("work", 3, 0, 0);
    TimedExercise *d = new TimedExercise("work", 4, 0, 0);
    model.appendExercise(a);
    model.appendExercise(b);
    model.appendExercise(c);
    model.appendExercise(d);

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
    TimedExercise *a = new TimedExercise("work", 1, 0, 0);
    TimedExercise *b = new TimedExercise("work", 2, 0, 0);
    TimedExercise *c = new TimedExercise("work", 3, 0, 0);
    TimedExercise *d = new TimedExercise("work", 4, 0, 0);
    TimedExercise *e = new TimedExercise("work", 5, 0, 0);
    model.appendExercise(a);
    model.appendExercise(b);
    model.appendExercise(c);
    model.appendExercise(d);
    model.appendExercise(e);

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
    TimedExercise *a = new TimedExercise("work", 1, 0, 0);
    TimedExercise *b = new TimedExercise("work", 2, 0, 0);
    TimedExercise *c = new TimedExercise("work", 3, 0, 0);
    model.appendExercise(a);
    model.appendExercise(b);
    model.appendExercise(c);

    // Copy A (index 0) and C (index 2) to target index 1.
    model.copyItems(QList<int>{0, 2}, 1);

    QCOMPARE(model.count(), 5);
    QCOMPARE(model.at(0), a);
    QVERIFY(model.at(1) != a);
    QCOMPARE(model.at(1)->durationSeconds(), a->durationSeconds());
    QVERIFY(model.at(2) != c);
    QCOMPARE(model.at(2)->durationSeconds(), c->durationSeconds());
    QCOMPARE(model.at(3), b);
    QCOMPARE(model.at(4), c);
}
