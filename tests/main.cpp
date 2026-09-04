#include <QCoreApplication>
#include <QStandardPaths>
#include <QtTest>

#include "tst_timedexercise.h"
#include "tst_exerciseset.h"
#include "tst_exerciselistmodel.h"
#include "tst_exercisetimer.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    // Isolate QSettings (used by ExerciseTimer) from the real application's
    // configuration so running tests never touches the user's saved settings.
    QStandardPaths::setTestModeEnabled(true);
    QCoreApplication::setOrganizationName("harbour-hortator-tests");
    QCoreApplication::setApplicationName("harbour-hortator-tests");

    int result = 0;

    TstTimedExercise timedExerciseTest;
    result |= QTest::qExec(&timedExerciseTest, argc, argv);

    TstExerciseSet exerciseSetTest;
    result |= QTest::qExec(&exerciseSetTest, argc, argv);

    TstExerciseListModel exerciseListModelTest;
    result |= QTest::qExec(&exerciseListModelTest, argc, argv);

    TstExerciseTimer exerciseTimerTest;
    result |= QTest::qExec(&exerciseTimerTest, argc, argv);

    return result;
}
