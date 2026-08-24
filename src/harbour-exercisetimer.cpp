#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include "timedexercise.h"
#include "exerciseset.h"
#include "exerciselistmodel.h"
#include "exercisetimer.h"
#include "workouthistory.h"
#include "exercisetemplatelibrary.h"
#include "eoqttrace.h"

#ifndef APP_VERSION
#define APP_VERSION "unknown"
#endif

int main(int argc, char *argv[])
{
    // SailfishApp::main() will display "qml/harbour-exercisetimer.qml", if you need more
    // control over initialization, you can use:
    //
    //   - SailfishApp::application(int, char *[]) to get the QGuiApplication *
    //   - SailfishApp::createView() to get a new QQuickView * instance
    //   - SailfishApp::pathTo(QString) to get a QUrl to a resource file
    //   - SailfishApp::pathToMainQml() to get a QUrl to the main QML file
    //
    // To display the view, call "show()" (will show fullscreen on device).
    qmlRegisterType<ExerciseTimer>("io.github.teekesti.exercisetimer", 1, 0, "ExerciseTimer");
    qmlRegisterType<ExerciseListModel>("io.github.teekesti.exercisetimer", 1, 0, "ExerciseListModel");
    qmlRegisterType<TimedExercise>("io.github.teekesti.exercisetimer", 1, 0, "TimedExercise");
    qmlRegisterUncreatableType<ExerciseSet>("io.github.teekesti.exercisetimer", 1, 0, "ExerciseSet",
            "ExerciseSet instances are created by ExerciseTimer, not from QML");
    qmlRegisterUncreatableType<WorkoutHistory>("io.github.teekesti.exercisetimer", 1, 0, "WorkoutHistory",
            "WorkoutHistory is created by ExerciseTimer, not from QML");
    qmlRegisterUncreatableType<ExerciseTemplateLibrary>("io.github.teekesti.exercisetimer", 1, 0,
            "ExerciseTemplateLibrary", "ExerciseTemplateLibrary is created by ExerciseTimer, not from QML");
    auto app = SailfishApp::application(argc, argv);
    auto view = SailfishApp::createView();
    QDir qmlDir = QDir(SailfishApp::pathTo("qml").toLocalFile());
    view->engine()->addImportPath(qmlDir.path());
    view->setSource(QUrl::fromLocalFile(qmlDir.filePath("harbour-exercisetimer.qml")));
    QQmlContext *context = view->rootContext();
    ExerciseTimer *exerciseTimer = new ExerciseTimer(app);
    ExerciseListModel *model = exerciseTimer->exerciseListModel();
    if (model->isEmpty())
    {
        // No Draft was restored from disk (fresh install, or the Draft
        // was emptied) - fall back to the same starting point as before
        // Draft persistence existed. Re-sync so this untouched default
        // doesn't read as having unsaved changes.
        exerciseTimer->appendDefaultSet();
        exerciseTimer->markDraftSynced();
    }
    TRACE1("model contains %1 items", model->size());
    context->setContextProperty("exerciseTimer", exerciseTimer);
    context->setContextProperty("exerciseListModel", model);
    context->setContextProperty("applicationDisplayName", app->applicationDisplayName());
    context->setContextProperty("appVersion", QString(APP_VERSION));
    //return SailfishApp::main(argc, argv);
    view->show();
    return app->exec();
}
