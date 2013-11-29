#-------------------------------------------------
#
# Project created by QtCreator 2013-10-28T09:35:37
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -std=c++11


TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

VERSION = 0.3
DEFINES += VERSIONSTR=\\\"$${VERSION}\\\"

unix: !macx {
	TARGET = mastermind
} else {
	TARGET = Mastermind
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

TRANSLATIONS = translations/mastermind_ara.ts \
	translations/mastermind_cz.ts \
	translations/mastermind_de.ts \
	translations/mastermind_en.ts \
	translations/mastermind_es.ts \
	translations/mastermind_fa.ts \
	translations/mastermind_fr.ts \
	translations/mastermindk_nl.ts

OTHER_FILES += \
	mastermind.desktop
