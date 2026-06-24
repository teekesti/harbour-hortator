# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-exercisetimer
QT += core multimedia

CONFIG += sailfishapp qml_debug

DEFINES += ENABLE_FUNC_TRACE ENABLE_TRACE

SOURCES += src/harbour-exercisetimer.cpp \
    src/exerciselistmodel.cpp \
    src/exerciseset.cpp \
    src/exercisetimer.cpp \
    src/soundplayer.cpp \
    src/timedexercise.cpp \
    src/workouthistory.cpp \
    src/exercisetemplatelibrary.cpp

DISTFILES += qml/harbour-exercisetimer.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/RunPage.qml \
    qml/pages/SettingsPage.qml \
    qml/pages/HistoryPage.qml \
    qml/pages/SaveWorkoutDialog.qml \
    qml/pages/ConfirmReplaceDraftDialog.qml \
    qml/pages/PickExerciseTemplateDialog.qml \
    qml/pages/ExerciseLibraryPage.qml \
    qml/pages/components/qmldir \
    qml/pages/components/AcceleratingIconButton.qml \
    qml/pages/components/ValueAdjustmentHorizontal.qml \
    qml/pages/components/RoundCountAdjustment.qml \
    qml/pages/components/SummaryBar.qml \
    rpm/harbour-exercisetimer.changes.in \
    rpm/harbour-exercisetimer.changes.run.in \
    rpm/harbour-exercisetimer.spec \
    translations/*.ts \
    harbour-exercisetimer.desktop \
    translations/harbour-exercisetimer-fi.ts

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-exercisetimer-de.ts translations/harbour-exercisetimer-fi.ts

HEADERS += \
    src/eoqttrace.h \
    src/exerciselistmodel.h \
    src/exerciseset.h \
    src/exercisetimer.h \
    src/soundplayer.h \
    src/timedexercise.h \
    src/workouthistory.h \
    src/exercisetemplatelibrary.h

# Target directory for sounds on device
sounds.path = /usr/share/$$TARGET/sounds

# Source files for sounds
sounds.files = sounds/*

# Add definition of sounds for RPM packaging
DEPLOYMENT += sounds
