/***********************************************************************
 *
 * Copyright (C) 2013 Omid Nikta <omidnikta@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "mainwindow.h"
#include "board.h"
#include "preferences.h"
#include <QApplication>
#include <QComboBox>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QSpinBox>
#include <QToolBar>
#include <QRadioButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLinearGradient>
#include <QString>
#include <QStyleOption>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	mMode = (GameMode) QSettings().value("Mode", 0).toInt();
	mColors = QSettings().value("Colors", 6).toInt()-2;
	mPegs = QSettings().value("Pegs", 4).toInt()-2;
	mAlgorithm = (Algorithm) QSettings().value("Algorithm", 0).toInt();
	mSameColorAllowed = QSettings().value("SameColor", true).toBool();
	mSetPins = QSettings().value("SetPins", true).toBool();
	mCloseRow = QSettings().value("CloseRow", true).toBool();
	mIndicator = (IndicatorType) QSettings().value("Indicator", 0).toInt();
	mLocale = QLocale(QSettings().value("Locale/Language", "en").toString().left(5));
	mFontName = QSettings().value("FontName", "SansSerif").toString();
	mFontSize = QSettings().value("FontSize", 12).toInt();

	mLocale.setNumberOptions(QLocale::OmitGroupSeparator);

	mBoard = new Board(mFontName, mFontSize, this);

	setLayoutDirection(mLocale.textDirection());
	setWindowTitle(tr("CodeBreak"));
	setCentralWidget(mBoard);
	connect(this, SIGNAL(changeIndicatorsSignal(IndicatorType)), mBoard, SLOT(onChangeIndicators(IndicatorType)));

	Preferences::loadTranslation("codebreak_");

	createMenuBar();
	restoreGeometry(QSettings().value("Geometry").toByteArray());
	updateNumbersSlot();
}
//-----------------------------------------------------------------------------

MainWindow::~MainWindow()
{
}
//-----------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* event)
{
	QSettings().setValue("Mode", (int) mMode);
	QSettings().setValue("Colors", mColors+2);
	QSettings().setValue("Pegs", mPegs+2);
	QSettings().setValue("Algorithm", (int) mAlgorithm);
	QSettings().setValue("SameColor", mSameColorAllowed);
	QSettings().setValue("SetPins",	mSetPins);
	QSettings().setValue("CloseRow", mCloseRow);
	QSettings().setValue("Indicator", (int) mIndicator);
	QSettings().setValue("Geometry", saveGeometry());
	QMainWindow::closeEvent(event);
}
//-----------------------------------------------------------------------------

void MainWindow::newGameSlot()
{
	resignAction->setVisible(mMode == GameMode::Master);
	mBoard->reset(mPegs+2, mColors+2, mMode, mSameColorAllowed, mAlgorithm, mSetPins, mCloseRow, mLocale, mIndicator);
	mBoard->play(mMode);
}
//-----------------------------------------------------------------------------

void MainWindow::onPreferences()
{
	auto preferencesWidget = new Preferences(&mLocale, this);
	preferencesWidget->setWindowTitle(tr("Preferences"));
	preferencesWidget->exec();
}
//-----------------------------------------------------------------------------

void MainWindow::allowSameColorSlot()
{
	mSameColorAllowed = (allowSameColorAction->isChecked()) || (mPegs > mColors);
	allowSameColorAction->setChecked(mSameColorAllowed);
	if(mSameColorAllowed) {
		allowSameColorAction->setText(tr("Same Color Allowed"));
	} else {
		allowSameColorAction->setText(tr("Same Color Not Allowed"));
	}
}
//-----------------------------------------------------------------------------

void MainWindow::setPinsCloseRowAutomatically()
{
	mSetPins = setPinsAutomaticallyAction->isChecked();
	mCloseRow = closeRowAutomaticallyAction->isChecked();
	mBoard->setPinsRow(mSetPins, mCloseRow);
}
//-----------------------------------------------------------------------------

void MainWindow::changeIndicatorsSlot(QAction* selectedAction)
{
	mIndicator = (IndicatorType) selectedAction->data().toInt();
	emit changeIndicatorsSignal(mIndicator);
}
//-----------------------------------------------------------------------------

void MainWindow::changeGameModeSlot(QAction* selectedAction)
{
	mMode =  (GameMode) selectedAction->data().toInt();
	updateNumbersSlot();
}

void MainWindow::updateNumbersSlot()
{
	mColors = colorsNumberComboBox->currentIndex();
	mAlgorithm = (Algorithm) solvingAlgorithmsComboBox->currentIndex();
	mPegs = pegsNumberComboBox->currentIndex();
	mSameColorAllowed = allowSameColorAction->isChecked();

	if(mBoard)
	{
		mBoard->setAlgorithm(mAlgorithm);
		if(mBoard->getState() != GameState::Running)
		{
			newGameSlot();
		}
	}

	QSettings().setValue("Mode", (int) mMode);
	QSettings().setValue("Colors", mColors+2);
	QSettings().setValue("Pegs", mPegs+2);
	QSettings().setValue("Algorithm", (int) mAlgorithm);
	QSettings().setValue("SetPins",	mSetPins);
	QSettings().setValue("CloseRow", mCloseRow);
	QSettings().setValue("Indicator", (int) mIndicator);
	QSettings().setValue("SameColor", mSameColorAllowed);
}
//-----------------------------------------------------------------------------

void MainWindow::about()
{
	QMessageBox::about(this, tr("About CodeBreak"), QString(
		"<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>"
		"<p align='center'>%6<br/><small>%7</small></p>")
		.arg(tr("CodeBreak"), QCoreApplication::applicationVersion(),
			tr("Code Breaking Game, A Clone Of The Mastermind Board Game"),
			tr("Copyright &copy; 2013-%1 Omid Nikta").arg(mLocale.toString(2013)),
			tr("Released under the <a href=%1>GPL 3</a> license").arg("\"http://www.gnu.org/licenses/gpl.html\""),
			tr("Uses icons from the <a href=%1>Oxygen</a> icon theme").arg("\"http://www.oxygen-icons.org/\""),
			tr("Used under the <a href=%1>LGPL 3</a> license").arg("\"http://www.gnu.org/licenses/lgpl.html\""))
	);
}

//-----------------------------------------------------------------------------

void MainWindow::setPegsComboBox()
{
	pegsNumberComboBox = new QComboBox(this);
	for(int i = MIN_SLOT_NUMBER; i <= MAX_SLOT_NUMBER; ++i)
	{
		pegsNumberComboBox->addItem(QString("%1 %2").arg(mLocale.toString(i)).arg(tr("Slots")));
	}
	pegsNumberComboBox->setCurrentIndex(mPegs);
	pegsNumberComboBox->setToolTip(tr("Choose the numbe of slots"));
	pegsNumberComboBox->setLocale(mLocale);

	connect(pegsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateNumbersSlot()));
}
//-----------------------------------------------------------------------------

void MainWindow::setColorsComboBox()
{
	colorsNumberComboBox = new QComboBox(this);
	for(int i = MIN_COLOR_NUMBER; i <= MAX_COLOR_NUMBER; ++i)
	{
		colorsNumberComboBox->addItem(QString("%1 %2").arg(mLocale.toString(i)).arg(tr("Colors")));
	}
	colorsNumberComboBox->setCurrentIndex(mColors);
	colorsNumberComboBox->setToolTip(tr("Choose the number of colors"));

	connect(colorsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateNumbersSlot()));
}
//-----------------------------------------------------------------------------

void MainWindow::setSolvingAlgorithmsComboBox()
{
	solvingAlgorithmsComboBox = new QComboBox(this);
	solvingAlgorithmsComboBox->setToolTip(tr("Choose the solving algorithm"));
	solvingAlgorithmsComboBox->addItem(tr("Most Parts"));
	solvingAlgorithmsComboBox->addItem(tr("Worst Case"));
	solvingAlgorithmsComboBox->addItem(tr("Expected Size"));
	solvingAlgorithmsComboBox->setCurrentIndex((int) mAlgorithm);

	connect(solvingAlgorithmsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateNumbersSlot()));
}
//-----------------------------------------------------------------------------

void MainWindow::createMenuBar()
{
	auto gameMenu = menuBar()->addMenu(tr("&Game"));

	QIcon new_icon(QPixmap(":icons/new-icon.png"));
	auto newGameAction = gameMenu->addAction(new_icon, tr("&New"), this, SLOT(newGameSlot()), tr("Ctrl+N"));

	QIcon resign_icon(QPixmap(":/icons/face-raspberry.png"));
	resignAction = gameMenu->addAction(resign_icon, tr("&Resign"), mBoard, SLOT(onResign()), tr("Ctrl+R"));
	resignAction->setVisible(mMode == GameMode::Master);

	gameMenu->addSeparator();

	QIcon close_icon(QPixmap(":/icons/close-icon.png"));
	gameMenu->addAction(close_icon, tr("&Quit"), this, SLOT(close()), tr("Ctrl+Q"));

	auto settingsMenu = menuBar()->addMenu(tr("&Settings"));

	auto gameModeSubMenu = settingsMenu->addMenu(tr("Game Mode"));
	auto gameModeActions = new QActionGroup(this);

	auto masterModeAction = gameModeSubMenu->addAction(tr("Code Master"));
	masterModeAction->setData(0);
	masterModeAction->setCheckable(true);
	masterModeAction->setChecked(mMode == GameMode::Master);
	gameModeActions->addAction(masterModeAction);

	auto breakerModeAction = gameModeSubMenu->addAction(tr("Code Breaker"));
	breakerModeAction->setData(1);
	breakerModeAction->setCheckable(true);
	breakerModeAction->setChecked(mMode == GameMode::Breaker);
	gameModeActions->addAction(breakerModeAction);

	gameModeActions->setExclusive(true);
	connect(gameModeActions, SIGNAL(triggered(QAction*)), this, SLOT(changeGameModeSlot(QAction*)));

	auto indicatorTypeSubMenu = settingsMenu->addMenu(tr("Indicator Type"));
	auto indicatorTypeActions = new QActionGroup(this);

	auto noIndicatorAction = indicatorTypeSubMenu->addAction(tr("None"));
	noIndicatorAction->setData(0);
	noIndicatorAction->setCheckable(true);
	noIndicatorAction->setChecked(mIndicator == IndicatorType::None);
	indicatorTypeActions->addAction(noIndicatorAction);

	auto charIndicatorAction = indicatorTypeSubMenu->addAction(tr("Characters"));
	charIndicatorAction->setData(1);
	charIndicatorAction->setCheckable(true);
	charIndicatorAction->setChecked(mIndicator == IndicatorType::Character);
	indicatorTypeActions->addAction(charIndicatorAction);

	auto digitIndicatorAction = indicatorTypeSubMenu->addAction(tr("Digits"));
	digitIndicatorAction->setData(2);
	digitIndicatorAction->setCheckable(true);
	digitIndicatorAction->setChecked(mIndicator == IndicatorType::Digit);
	indicatorTypeActions->addAction(digitIndicatorAction);

	indicatorTypeActions->setExclusive(true);
	connect(indicatorTypeActions, SIGNAL(triggered(QAction*)), this, SLOT(changeIndicatorsSlot(QAction*)));

	setPinsAutomaticallyAction = settingsMenu->addAction(tr("Set Pins Automatically"), this, SLOT(setPinsCloseRowAutomatically()));
	setPinsAutomaticallyAction->setCheckable(true);
	setPinsAutomaticallyAction->setChecked(mSetPins == 1);

	closeRowAutomaticallyAction = settingsMenu->addAction(tr("Close Row Automatically"), this, SLOT(setPinsCloseRowAutomatically()));
	closeRowAutomaticallyAction->setCheckable(true);
	closeRowAutomaticallyAction->setChecked(mCloseRow == 1);

	settingsMenu->addAction(tr("Preferences"), this, SLOT(onPreferences()));

	auto helpMenu = menuBar()->addMenu(tr("&Help"));

	QIcon information_icon(QPixmap(":/icons/information-icon.png"));
	helpMenu->addAction(information_icon, tr("About"), this, SLOT(about()));

	QIcon qt_icon(QPixmap(":/icons/qt_icon.png"));
	helpMenu->addAction(qt_icon, tr("About Qt"), qApp, SLOT(aboutQt()));

	auto mainToolbar = new QToolBar(this);
	mainToolbar->setFloatable(false);
	mainToolbar->setMovable(false);
	mainToolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
	mainToolbar->addAction(newGameAction);

	QIcon double_icon;
	double_icon.addPixmap(QPixmap(":/icons/same_color_1.png"), QIcon::Normal, QIcon::On);
	double_icon.addPixmap(QPixmap(":/icons/same_color_0.png"), QIcon::Normal, QIcon::Off);
	allowSameColorAction = new QAction(double_icon, tr("Same Color Allowed"), this);
	allowSameColorAction->setCheckable(true);
	allowSameColorAction->setChecked(mSameColorAllowed);
	connect(allowSameColorAction, SIGNAL(triggered()), this, SLOT(allowSameColorSlot()));
	connect(allowSameColorAction, SIGNAL(triggered()), this, SLOT(updateNumbersSlot()));
	allowSameColorSlot();

	mainToolbar->addAction(allowSameColorAction);
	mainToolbar->addSeparator();

	setPegsComboBox();
	setColorsComboBox();
	setSolvingAlgorithmsComboBox();

	mainToolbar->addWidget(pegsNumberComboBox);
	mainToolbar->addWidget(colorsNumberComboBox);
	mainToolbar->addWidget(solvingAlgorithmsComboBox);
	addToolBar(mainToolbar);
	setContextMenuPolicy(Qt::NoContextMenu);
}
