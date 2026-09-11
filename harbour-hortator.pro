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
TARGET = harbour-hortator
QT += core multimedia

CONFIG += sailfishapp qml_debug

VERSION = $$system(grep '^Version:' rpm/harbour-hortator.spec | sed 's/Version:[[:space:]]*//')
DEFINES += ENABLE_FUNC_TRACE ENABLE_TRACE APP_VERSION=\\\"$$VERSION\\\"

SOURCES += src/harbour-hortator.cpp \
    src/exerciselistmodel.cpp \
    src/exerciseset.cpp \
    src/exercisetimer.cpp \
    src/soundplayer.cpp \
    src/timedexercise.cpp \
    src/workouthistory.cpp \
    src/exercisetemplatelibrary.cpp

DISTFILES += qml/harbour-hortator.qml \
    qml/cover/CoverPage.qml \
    qml/pages/AboutPage.qml \
    qml/pages/SoundCreditsPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/RunPage.qml \
    qml/pages/SettingsPage.qml \
    qml/pages/HistoryPage.qml \
    qml/pages/SaveWorkoutDialog.qml \
    qml/pages/ConfirmReplaceDraftDialog.qml \
    qml/pages/PickExerciseTemplateDialog.qml \
    qml/pages/EditExerciseDialog.qml \
    qml/pages/ExerciseLibraryPage.qml \
    qml/pages/components/qmldir \
    qml/pages/components/AcceleratingIconButton.qml \
    qml/pages/components/ValueAdjustmentHorizontal.qml \
    qml/pages/components/RoundCountAdjustment.qml \
    qml/pages/components/SummaryBar.qml \
    qml/pages/components/UIConstants.qml \
    qml/pages/components/ActivityProgressBar.qml \
    qml/pages/components/TotalProgressBar.qml \
    qml/images/kettlebell_stopwatch_with_background.png \
    rpm/harbour-hortator.changes.in \
    rpm/harbour-hortator.changes.run.in \
    rpm/harbour-hortator.spec \
    sounds/bell-high_C.wav \
    sounds/bell-low_C.wav \
    sounds/big_fanfare_in_C.wav \
    sounds/countdown_bell_G.wav \
    sounds/vocoder_whipbell.wav \
    translations/*.ts \
    harbour-hortator.desktop \
    translations/harbour-hortator-fi.ts

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

TRANSLATIONS += translations/harbour-hortator-de.ts translations/harbour-hortator-fi.ts

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
