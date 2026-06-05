#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include "timedexercise.h"
#include "exerciselistmodel.h"
#include "exercisetimer.h"
#include "eoqttrace.h"

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
    qmlRegisterType<ExerciseTimer>("com.appiukko.exercisetimer", 1, 0, "ExerciseTimer");
    qmlRegisterType<ExerciseListModel>("com.appiukko.exercisetimer", 1, 0, "ExerciseListModel");
    qmlRegisterType<TimedExercise>("com.appiukko.exercisetimer", 1, 0, "TimedExercise");
    auto app = SailfishApp::application(argc, argv);
    auto view = SailfishApp::createView();
    QDir qmlDir = QDir(SailfishApp::pathTo("qml").toLocalFile());
    view->setSource(QUrl::fromLocalFile(qmlDir.filePath("harbour-exercisetimer.qml")));
    QQmlContext *context = view->rootContext();
    ExerciseTimer *exerciseTimer = new ExerciseTimer(app);
    exerciseTimer->appendDefaultExercise();
    ExerciseListModel *model = exerciseTimer->exerciseListModel();
    TRACE1("model contains %1 items", model->size());
    context->setContextProperty("exerciseTimer", exerciseTimer);
    context->setContextProperty("exerciseListModel", model);
    //return SailfishApp::main(argc, argv);
    view->show();
    return app->exec();
}
