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
	TARGET = codebreak
} else {
	TARGET = CodeBreak
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
    src/preferences.ui

RESOURCES += \
    resource.qrc

TRANSLATIONS = translations/codebreak_ara.ts \
	translations/codebreak_cz.ts \
	translations/codebreak_de.ts \
	translations/codebreak_en.ts \
	translations/codebreak_es.ts \
	translations/codebreak_fa.ts \
	translations/codebreak_fr.ts \
	translations/codebreak_nl.ts
