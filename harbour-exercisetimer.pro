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

CONFIG += sailfishapp

SOURCES += src/harbour-exercisetimer.cpp \
    src/exerciselistmodel.cpp \
    src/exercisetimer.cpp \
    src/soundplayer.cpp \
    src/timedexercise.cpp

DISTFILES += qml/harbour-exercisetimer.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/SecondPage.qml \
    rpm/harbour-exercisetimer.changes.in \
    rpm/harbour-exercisetimer.changes.run.in \
    rpm/harbour-exercisetimer.spec \
    sounds/61234__sapht__snes-startup.wav \
    sounds/62176__robinhood76__00504-brass-fanfare-4.wav \
    sounds/beep.wav \
    sounds/boxing-bell-1.wav \
    sounds/boxing-bell-3.wav \
    sounds/eight.wav \
    sounds/five.wav \
    sounds/four.wav \
    sounds/nine.wav \
    sounds/one.wav \
    sounds/seven.wav \
    sounds/six.wav \
    sounds/ten.wav \
    sounds/three.wav \
    sounds/two.wav \
    sounds/zero.wav \
    translations/*.ts \
    harbour-exercisetimer.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-exercisetimer-de.ts

HEADERS += \
    src/eoqttrace.h \
    src/exerciselistmodel.h \
    src/exercisetimer.h \
    src/soundplayer.h \
    src/timedexercise.h
