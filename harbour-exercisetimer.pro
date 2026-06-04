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
    sounds/* \
    rpm/harbour-exercisetimer.changes.in \
    rpm/harbour-exercisetimer.changes.run.in \
    rpm/harbour-exercisetimer.spec \
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

# Määritetään kohdehakemisto laitteella (esim. /usr/share/harbour-exercise-timer/sounds)
sounds.path = /usr/share/$$TARGET/sounds

# Kerrotaan mitkä tiedostot lähdekoodista otetaan mukaan
sounds.files = sounds/*

# Lisätään määrittely DEPLOYMENT-muuttujaan, jotta se otetaan huomioon RPM-pakettia luodessa
DEPLOYMENT += sounds
