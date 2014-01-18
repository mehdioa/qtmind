#-------------------------------------------------
#
# Project created by QtCreator 2013-10-28T09:35:37
#
#-------------------------------------------------

QT	   += core gui

QMAKE_CXXFLAGS += -std=c++0x

TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

VERSION = 0.7.2
DEFINES += VERSIONSTR=\\\"$${VERSION}\\\"

#unix: !macx {
#	TARGET = qtmind
#} else {
#	TARGET = QtMind
#}

TEMPLATE = app


SOURCES += src/main.cpp\
	src/mainwindow.cpp \
	src/peg.cpp \
	src/button.cpp \
	src/pegbox.cpp \
	src/emptybox.cpp \
	src/message.cpp \
	src/pinbox.cpp \
	src/pin.cpp \
	src/preferences.cpp \
	src/solver.cpp \
	src/game.cpp \
	src/gamerules.cpp \
	src/indicator.cpp \
	src/guesselement.cpp \
	src/boardaid.cpp \
	src/boardsounds.cpp \
	src/boardfont.cpp

HEADERS  += src/mainwindow.h \
	src/peg.h \
	src/constants.h \
	src/button.h \
	src/pegbox.h \
	src/emptybox.h \
	src/message.h \
	src/pinbox.h \
	src/pin.h \
	src/preferences.h \
	src/solver.h \
	src/game.h \
	src/appinfo.h \
	src/gamerules.h \
	src/indicator.h \
	src/guesselement.h \
	src/boardaid.h \
	src/boardsounds.h \
	src/boardfont.h

FORMS	+= \
	src/preferences.ui \
	src/mainwindow.ui

RESOURCES += \
	resource.qrc

TRANSLATIONS = translations/QtMind_ara.ts \
	translations/QtMind_af.ts \
	translations/QtMind_ar.ts \
	translations/QtMind_ca.ts \
	translations/QtMind_cs.ts \
	translations/QtMind_cz.ts \
	translations/QtMind_de.ts \
	translations/QtMind_en.ts \
	translations/QtMind_es.ts \
	translations/QtMind_fa.ts \
	translations/QtMind_fo.ts \
	translations/QtMind_fr.ts \
	translations/QtMind_it.ts \
	translations/QtMind_ja.ts \
	translations/QtMind_ko.ts \
	translations/QtMind_nl.ts \
	translations/QtMind_pl.ts \
	translations/QtMind_pt_BR.ts \
	translations/QtMind_ru.ts \
	translations/QtMind_sl.ts \
	translations/QtMind_tr.ts \
	translations/QtMind_zh_CN.ts

OTHER_FILES += \
	QtMind.desktop \
	android/AndroidManifest.xml \
	icons/hicolor/16x16/QtMind.png \
	icons/hicolor/32x32/QtMind.png \
	icons/hicolor/36x36/QtMind.png \
	icons/hicolor/48x48/QtMind.png \
	icons/hicolor/64x64/QtMind.png \
	icons/hicolor/72x72/QtMind.png \
	icons/hicolor/96x96/QtMind.png \
	icons/hicolor/144x144/QtMind.png \
	icons/hicolor/256x256/QtMind.png \
	icons/hicolor/512x512/QtMind.png \
	icons/hicolor/scalable/QtMind.svg \
	icons/hicolor/scalable/twoPegs.svg \
	icons/hicolor/scalable/logo.svg \
	QtMind.pbs \
    QtMind.qbs \
    src/src.qbs

unix:!macx { # installation on Unix-ish platforms
	isEmpty(INSTALL_PREFIX):INSTALL_PREFIX = /usr
	isEmpty(BIN_DIR):BIN_DIR = $$INSTALL_PREFIX/bin
	isEmpty(DATA_DIR):DATA_DIR = $$INSTALL_PREFIX/share
	isEmpty(ICON_DIR):ICON_DIR = $$DATA_DIR/pixmaps
	isEmpty(DESKTOP_DIR):DESKTOP_DIR = $$DATA_DIR/applications
	isEmpty(TRANSLATIONS_DIR):TRANSLATIONS_DIR = $$DATA_DIR/QtMind/translations

	target.path = $$BIN_DIR
	icon.files = icons/hicolor/128x128/QtMind.png
	icon.path = $$ICON_DIR
	desktop.files = QtMind.desktop
	desktop.path = $$DESKTOP_DIR
	qm.files = translations/*.qm
	qm.path = $$TRANSLATIONS_DIR

	INSTALLS = target icon desktop qm
}

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

ANDROID_EXTRA_LIBS = 

