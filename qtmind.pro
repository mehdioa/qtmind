#-------------------------------------------------
#
# Project created by QtCreator 2013-10-28T09:35:37
#
#-------------------------------------------------

QT       += core gui multimedia

QMAKE_CXXFLAGS += -std=c++0x

TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

VERSION = 0.5.5
DEFINES += VERSIONSTR=\\\"$${VERSION}\\\"

unix: !macx {
	TARGET = qtmind
} else {
	TARGET = QtMind
}

TEMPLATE = app


SOURCES += src/main.cpp\
		src/mainwindow.cpp \
	src/peg.cpp \
	src/board.cpp \
	src/button.cpp \
	src/pegbox.cpp \
    src/emptybox.cpp \
    src/message.cpp \
    src/game.cpp \
    src/pinbox.cpp \
    src/pin.cpp \
    src/preferences.cpp

HEADERS  += src/mainwindow.h \
	src/peg.h \
	src/constants.h \
	src/board.h \
	src/button.h \
	src/pegbox.h \
    src/emptybox.h \
    src/message.h \
    src/game.h \
    src/pinbox.h \
    src/pin.h \
    src/preferences.h

FORMS    += \
    src/preferences.ui \
    src/mainwindow.ui

RESOURCES += \
    resource.qrc

TRANSLATIONS = translations/qtmind_ara.ts \
	translations/qtmind_cz.ts \
	translations/qtmind_de.ts \
	translations/qtmind_en.ts \
	translations/qtmind_es.ts \
	translations/qtmind_fa.ts \
	translations/qtmind_fr.ts \
	translations/qtmindk_nl.ts

OTHER_FILES += \
	qtmind.desktop
