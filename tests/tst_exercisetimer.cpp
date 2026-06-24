#include "tst_exercisetimer.h"

#include <QtTest>
#include <QSettings>
#include <QFile>
#include <QVariantMap>
#include "exercisetimer.h"
#include "exerciselistmodel.h"
#include "exerciseset.h"
#include "timedexercise.h"
#include "workouthistory.h"
#include "exercisetemplatelibrary.h"

namespace {
ExerciseSet *setWithOneExercise(QString activityType, int mins, int secs)
{
    ExerciseSet *set = new ExerciseSet();
    set->appendExercise(new TimedExercise(activityType, mins, secs, 0));
    return set;
}
}

void TstExerciseTimer::init()
{
    // ExerciseTimer reads/writes "start delay" and "end notification time"
    // via QSettings. Clear them before each test so defaults are
    // deterministic and tests don't leak settings into one another.
    // tests/main.cpp sets a dedicated, test-mode QSettings namespace.
    QSettings().clear();
    // Likewise for the Draft/History JSON files: tests/main.cpp puts
    // QStandardPaths in test mode, but that redirects to one shared
    // location for the whole test run, not a fresh one per test.
    QFile::remove(ExerciseTimer::draftFilePath());
    QFile::remove(WorkoutHistory::defaultStorageFilePath());
    QFile::remove(ExerciseTemplateLibrary::defaultStorageFilePath());
}

void TstExerciseTimer::durationAggregation()
{
    ExerciseTimer timer(nullptr, false);
    timer.addSet(setWithOneExercise("work", 1, 0));  // 60 s
    timer.addSet(setWithOneExercise("rest", 0, 30)); // 30 s

    QCOMPARE(timer.totalDuration(), QTime(0, 1, 30));

    timer.removeSet(0);

    QCOMPARE(timer.totalDuration(), QTime(0, 0, 30));
}

void TstExerciseTimer::validityReflectsExercises()
{
    ExerciseTimer timer(nullptr, false);
    QVERIFY(!timer.allExercisesValid());

    timer.addSet(setWithOneExercise("work", 1, 0)); // valid
    QVERIFY(timer.allExercisesValid());

    timer.addSet(setWithOneExercise("work", 0, 0)); // invalid (zero duration)
    QVERIFY(!timer.allExercisesValid());

    timer.removeSet(1); // remove the invalid one
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

void TstExerciseTimer::muteSoundsToggleAndPersists()
{
    ExerciseTimer timer(nullptr, false);
    QCOMPARE(timer.muteSounds(), false);

    QSignalSpy spy(&timer, &ExerciseTimer::muteSoundsChanged);
    timer.setMuteSounds(true);
    QCOMPARE(timer.muteSounds(), true);
    QCOMPARE(spy.count(), 1);

    timer.setMuteSounds(true);
    QCOMPARE(spy.count(), 1);

    ExerciseTimer freshTimer(nullptr, false);
    QCOMPARE(freshTimer.muteSounds(), true);
}

void TstExerciseTimer::namingExerciseUpsertsTemplate()
{
    ExerciseTimer timer(nullptr, false);
    timer.addSet(new ExerciseSet());
    timer.addExerciseToSet(0, new TimedExercise("work", 2, 30, 0));
    TimedExercise *exercise = timer.exerciseListModel()->at(0)->at(0);

    QVERIFY(!timer.templateLibrary()->hasTemplate("Push-ups"));
    exercise->setName("Push-ups");

    QVERIFY(timer.templateLibrary()->hasTemplate("Push-ups"));
    QVariantMap tmpl = timer.templateLibrary()->templateByName("Push-ups");
    QCOMPARE(tmpl.value("mins").toInt(), 2);
    QCOMPARE(tmpl.value("secs").toInt(), 30);

    // Editing the named exercise's duration re-upserts the template.
    exercise->setMins(5);
    tmpl = timer.templateLibrary()->templateByName("Push-ups");
    QCOMPARE(tmpl.value("mins").toInt(), 5);
}

void TstExerciseTimer::renamingExerciseDoesNotTouchOldTemplate()
{
    ExerciseTimer timer(nullptr, false);
    timer.addSet(new ExerciseSet());
    timer.addExerciseToSet(0, new TimedExercise("work", 1, 0, 0));
    TimedExercise *exercise = timer.exerciseListModel()->at(0)->at(0);

    exercise->setName("A");
    QVERIFY(timer.templateLibrary()->hasTemplate("A"));

    exercise->setName("B");
    QVERIFY(timer.templateLibrary()->hasTemplate("B"));
    // Renaming doesn't rename or remove the old template (ADR-0009).
    QVERIFY(timer.templateLibrary()->hasTemplate("A"));
}

void TstExerciseTimer::addExerciseFromTemplateCopiesByValue()
{
    ExerciseTimer timer(nullptr, false);
    timer.addSet(new ExerciseSet());
    timer.addExerciseToSet(0, new TimedExercise("work", 3, 0, 0));
    timer.exerciseListModel()->at(0)->at(0)->setName("Squats");

    timer.addExerciseToSetFromTemplate(0, "Squats");
    QCOMPARE(timer.exerciseListModel()->at(0)->count(), 2);
    TimedExercise *copy = timer.exerciseListModel()->at(0)->at(1);
    QCOMPARE(copy->name(), QString("Squats"));
    QCOMPARE(copy->mins(), 3);

    // Editing the copy doesn't change the template...
    copy->setMins(10);
    QCOMPARE(timer.templateLibrary()->templateByName("Squats").value("mins").toInt(), 3);

    // ...and editing the template (via the original) doesn't retroactively
    // change the copy.
    timer.exerciseListModel()->at(0)->at(0)->setMins(7);
    QCOMPARE(copy->mins(), 10);
}

void TstExerciseTimer::removingTemplateDoesNotAffectExistingExercise()
{
    ExerciseTimer timer(nullptr, false);
    timer.addSet(new ExerciseSet());
    timer.addExerciseToSet(0, new TimedExercise("work", 1, 0, 0));
    timer.exerciseListModel()->at(0)->at(0)->setName("Lunges");
    timer.addExerciseToSetFromTemplate(0, "Lunges");

    timer.templateLibrary()->removeTemplate("Lunges");
    QVERIFY(!timer.templateLibrary()->hasTemplate("Lunges"));

    TimedExercise *copy = timer.exerciseListModel()->at(0)->at(1);
    QCOMPARE(copy->name(), QString("Lunges"));
    QCOMPARE(copy->mins(), 1);
}

void TstExerciseTimer::resetOnFreshTimer()
{
    ExerciseTimer timer(nullptr, false);
    timer.addSet(setWithOneExercise("work", 1, 0));
    timer.addSet(setWithOneExercise("rest", 0, 30));

    QSignalSpy activitySpy(&timer, &ExerciseTimer::currentActivityChanged);
    QSignalSpy durationSpy(&timer, &ExerciseTimer::currentDurationChanged);
    QSignalSpy currentRunningSpy(&timer, &ExerciseTimer::currentRunningTimeChanged);
    QSignalSpy totalRunningSpy(&timer, &ExerciseTimer::totalRunningTimeChanged);

    timer.reset();

    QCOMPARE(timer.currentRunningTime(), QTime(0, 0, 0));
    QCOMPARE(timer.totalRunningTime(), QTime(0, 0, 0));
    QCOMPARE(timer.currentActivity(), timer.exerciseListModel()->at(0)->at(0));
    QCOMPARE(activitySpy.count(), 1);
    QCOMPARE(durationSpy.count(), 1);
    QCOMPARE(currentRunningSpy.count(), 1);
    QCOMPARE(totalRunningSpy.count(), 1);
}

void TstExerciseTimer::currentActivityOnEmptyModel()
{
    ExerciseTimer timer(nullptr, false);
    QCOMPARE(timer.currentActivity(), static_cast<TimedExercise *>(nullptr));

    timer.addSet(setWithOneExercise("work", 1, 0));
    QCOMPARE(timer.currentActivity(), timer.exerciseListModel()->at(0)->at(0));
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
    timer.addSet(setWithOneExercise("work", 0, 1));

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

void TstExerciseTimer::emptySetBlocksValidity()
{
    ExerciseTimer timer(nullptr, false);
    ExerciseSet *set = new ExerciseSet(); // empty
    timer.addSet(set);

    QVERIFY(!timer.allExercisesValid());

    timer.addExerciseToSet(0, new TimedExercise("work", 1, 0, 0));
    QVERIFY(timer.allExercisesValid());
}

void TstExerciseTimer::noSetsBlocksValidity()
{
    ExerciseTimer timer(nullptr, false);
    QVERIFY(!timer.allExercisesValid());

    timer.addSet(setWithOneExercise("work", 1, 0));
    QVERIFY(timer.allExercisesValid());

    timer.removeSet(0);
    QVERIFY(!timer.allExercisesValid());
}

void TstExerciseTimer::durationAccountsForSetAndExerciseRounds()
{
    ExerciseTimer timer(nullptr, false);
    ExerciseSet *set = new ExerciseSet();
    set->setRounds(2);
    TimedExercise *a = new TimedExercise("work", 1, 0, 0); // 60 s
    a->setRounds(3);
    TimedExercise *b = new TimedExercise("rest", 0, 30, 0); // 30 s, 1 round
    set->appendExercise(a);
    set->appendExercise(b);
    timer.addSet(set);

    // (60*3 + 30) * 2 set rounds = 420 s = 7 min
    QCOMPARE(timer.totalDuration(), QTime(0, 7, 0));
}

void TstExerciseTimer::playSequenceFlattensSetAndExerciseRounds()
{
    ExerciseTimer timer(nullptr, false);
    timer.setStartDelay(0); // start immediately, no countdown

    ExerciseSet *set1 = new ExerciseSet();
    set1->setRounds(2);
    TimedExercise *a = new TimedExercise("work", 0, 1, 0);
    a->setRounds(2);
    set1->appendExercise(a);
    timer.addSet(set1);

    ExerciseSet *set2 = new ExerciseSet();
    TimedExercise *b = new TimedExercise("work", 0, 1, 0);
    set2->appendExercise(b);
    timer.addSet(set2);

    timer.start();

    // Play 1: set1, set round 1, exercise round 1
    QCOMPARE(timer.currentActivity(), a);
    QCOMPARE(timer.currentSetNumber(), 1);
    QCOMPARE(timer.currentSetCount(), 2);
    QCOMPARE(timer.currentSetRoundNumber(), 1);
    QCOMPARE(timer.currentSetRoundCount(), 2);
    QCOMPARE(timer.currentExerciseRoundNumber(), 1);
    QCOMPARE(timer.currentExerciseRoundCount(), 2);

    // Play 2: set1, set round 1, exercise round 2
    QMetaObject::invokeMethod(&timer, "onCurrentExerciseFinished");
    QCOMPARE(timer.currentActivity(), a);
    QCOMPARE(timer.currentSetRoundNumber(), 1);
    QCOMPARE(timer.currentExerciseRoundNumber(), 2);

    // Play 3: set1, set round 2, exercise round resets to 1
    QMetaObject::invokeMethod(&timer, "onCurrentExerciseFinished");
    QCOMPARE(timer.currentActivity(), a);
    QCOMPARE(timer.currentSetRoundNumber(), 2);
    QCOMPARE(timer.currentExerciseRoundNumber(), 1);

    // Play 4: set1, set round 2, exercise round 2
    QMetaObject::invokeMethod(&timer, "onCurrentExerciseFinished");
    QCOMPARE(timer.currentActivity(), a);
    QCOMPARE(timer.currentSetRoundNumber(), 2);
    QCOMPARE(timer.currentExerciseRoundNumber(), 2);

    // Play 5: set2, exercise b
    QMetaObject::invokeMethod(&timer, "onCurrentExerciseFinished");
    QCOMPARE(timer.currentActivity(), b);
    QCOMPARE(timer.currentSetNumber(), 2);
    QCOMPARE(timer.currentSetRoundNumber(), 1);
    QCOMPARE(timer.currentSetRoundCount(), 1);
    QCOMPARE(timer.currentExerciseRoundNumber(), 1);
    QCOMPARE(timer.currentExerciseRoundCount(), 1);

    QSignalSpy finishedSpy(&timer, &ExerciseTimer::allExercisesFinished);
    QMetaObject::invokeMethod(&timer, "onCurrentExerciseFinished");
    QCOMPARE(finishedSpy.count(), 1);
}

void TstExerciseTimer::positionPropertiesDuringPlayback()
{
    ExerciseTimer timer(nullptr, false);
    timer.appendDefaultSet();

    QCOMPARE(timer.currentSetNumber(), 1);
    QCOMPARE(timer.currentSetCount(), 1);
    QCOMPARE(timer.currentSetRoundNumber(), 1);
    QCOMPARE(timer.currentSetRoundCount(), 1);
    QCOMPARE(timer.currentExerciseRoundNumber(), 1);
    QCOMPARE(timer.currentExerciseRoundCount(), 1);
}

void TstExerciseTimer::draftPersistsAcrossRestart()
{
    ExerciseTimer timer(nullptr, false);
    timer.addSet(setWithOneExercise("work", 2, 15));
    QMetaObject::invokeMethod(&timer, "saveDraftNow");

    ExerciseTimer freshTimer(nullptr, false);
    QCOMPARE(freshTimer.exerciseListModel()->count(), 1);
    QCOMPARE(freshTimer.exerciseListModel()->at(0)->count(), 1);
    TimedExercise *restored = freshTimer.exerciseListModel()->at(0)->at(0);
    QCOMPARE(restored->activityType(), QString("work"));
    QCOMPARE(restored->mins(), 2);
    QCOMPARE(restored->secs(), 15);
}

void TstExerciseTimer::draftDirtyTracksEdits()
{
    ExerciseTimer timer(nullptr, false);
    QVERIFY(!timer.isDraftDirty()); // freshly constructed, empty, synced

    timer.addSet(setWithOneExercise("work", 1, 0));
    QVERIFY(timer.isDraftDirty());
}

void TstExerciseTimer::saveDraftToHistoryRequiresValidity()
{
    ExerciseTimer timer(nullptr, false);
    ExerciseSet *invalidSet = new ExerciseSet(); // empty, invalid
    timer.addSet(invalidSet);
    QVERIFY(!timer.allExercisesValid());

    QVERIFY(!timer.saveDraftToHistory("Leg day"));
    QCOMPARE(timer.history()->count(), 0);
}

void TstExerciseTimer::saveDraftToHistorySucceedsAndClearsDirty()
{
    ExerciseTimer timer(nullptr, false);
    timer.addSet(setWithOneExercise("work", 1, 0));
    QVERIFY(timer.isDraftDirty());

    QVERIFY(timer.saveDraftToHistory("Leg day"));
    QCOMPARE(timer.history()->count(), 1);
    QVERIFY(!timer.isDraftDirty());

    // The draft itself is untouched by saving (ADR-0006) - still there,
    // still editable.
    QCOMPARE(timer.exerciseListModel()->count(), 1);
}

void TstExerciseTimer::loadHistoryEntryReplacesDraftAndMarksDirtyClean()
{
    ExerciseTimer timer(nullptr, false);
    timer.addSet(setWithOneExercise("work", 3, 0));
    QVERIFY(timer.saveDraftToHistory("Original"));
    QString entryId = timer.history()->data(timer.history()->index(0),
                                             WorkoutHistory::IdRole).toString();

    // Diverge the draft from what was saved.
    timer.addSet(setWithOneExercise("rest", 0, 20));
    QVERIFY(timer.isDraftDirty());

    timer.loadHistoryEntry(entryId);

    QVERIFY(!timer.isDraftDirty());
    QCOMPARE(timer.exerciseListModel()->count(), 1);
    QCOMPARE(timer.exerciseListModel()->at(0)->at(0)->mins(), 3);
}

void TstExerciseTimer::historySortsMostRecentlyPlayedFirst()
{
    ExerciseTimer timer(nullptr, false);
    timer.addSet(setWithOneExercise("work", 1, 0));
    QVERIFY(timer.saveDraftToHistory("First"));
    QString firstId = timer.history()->data(timer.history()->index(0),
                                             WorkoutHistory::IdRole).toString();

    QVERIFY(timer.saveDraftToHistory("Second"));
    QCOMPARE(timer.history()->count(), 2);

    // Marking the entry created first as played moves it ahead of the
    // never-played entry, regardless of creation order (ADR-0015).
    timer.history()->markPlayed(firstId);
    QCOMPARE(timer.history()->data(timer.history()->index(0),
                                    WorkoutHistory::NameRole).toString(),
             QString("First"));
}

void TstExerciseTimer::outOfRangeSetIndexIsNoOp()
{
    ExerciseTimer timer(nullptr, false);
    timer.addSet(setWithOneExercise("work", 1, 0));

    QSignalSpy modifySpy(&timer, &ExerciseTimer::requestModificationOfExercise);

    timer.addExerciseToSet(5, new TimedExercise("work", 1, 0, 0));
    timer.removeExerciseFromSet(-1, 0);
    timer.modifyExerciseInSet(5, 0);
    timer.modifyExerciseInSet(0, 5);

    QCOMPARE(timer.exerciseListModel()->count(), 1);
    QCOMPARE(timer.exerciseListModel()->at(0)->count(), 1);
    QCOMPARE(modifySpy.count(), 0);
}
