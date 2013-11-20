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

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	mBoard = new Board(this);
	mMode = QSettings().value("Mode", 0).toInt();
	mColors = QSettings().value("Colors", 6).toInt()-2;
	mPegs = QSettings().value("Pegs", 4).toInt()-2;
	mAlgorithm = QSettings().value("Algorithm", 0).toInt();
	mSameColorAllowed = QSettings().value("SameColor", true).toBool();
	mSetPins = QSettings().value("SetPins", true).toBool();
	mCloseRow = QSettings().value("CloseRow", true).toBool();
	mIndicator = QSettings().value("Indicator", 0).toInt();

	setWindowTitle(tr("CodeBreak"));
	setCentralWidget(mBoard);

	createMenuBar();

	connect(this, SIGNAL(changeIndicatorsSignal(int)), mBoard, SLOT(onChangeIndicators(int)));
	connect(doItForMeAction, SIGNAL(triggered()), mBoard, SLOT(onDoItForMe()));



	resize(400,400);
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
	QSettings().setValue("Mode", mMode);
	QSettings().setValue("Colors", mColors+2);
	QSettings().setValue("Pegs", mPegs+2);
	QSettings().setValue("Algorithm", mAlgorithm);
	QSettings().setValue("SameColor", mSameColorAllowed);
	QSettings().setValue("SetPins",	mSetPins);
	QSettings().setValue("CloseRow", mCloseRow);
	QSettings().setValue("Indicator", mIndicator);
	QSettings().setValue("Geometry", saveGeometry());
	QMainWindow::closeEvent(event);
}
//-----------------------------------------------------------------------------

void MainWindow::newGameSlot()
{
	throwInTheTowelAction->setVisible(mMode == MODE_MASTER);
	mBoard->reset(mPegs+2, mColors+2, mMode, mSameColorAllowed, mAlgorithm, mSetPins, mCloseRow, mIndicator);
	mBoard->play(mMode);
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
	mIndicator = selectedAction->data().toInt();
	emit changeIndicatorsSignal(mIndicator);
}
//-----------------------------------------------------------------------------

void MainWindow::doItForMeSlot()
{
}
//-----------------------------------------------------------------------------

void MainWindow::changeGameModeSlot(QAction* selectedAction)
{
	mMode = selectedAction->data().toInt();
	updateNumbersSlot();
}

void MainWindow::updateNumbersSlot()
{
	mColors = colorsNumberComboBox->currentIndex();
	mAlgorithm = solvingAlgorithmsComboBox->currentIndex();
	mPegs = pegsNumberComboBox->currentIndex();
	mSameColorAllowed = allowSameColorAction->isChecked();

	if(mBoard)
	{
		mBoard->setAlgorithm(mAlgorithm);
		if(mBoard->getState() != STATE_RUNNING)
		{
			newGameSlot();
		}
	}

	if(mMode == MODE_MASTER) {
		doItForMeAction->setText(tr("Put Pegs For Me"));
	} else {
		doItForMeAction->setText(tr("Put Pins For Me"));
	}

	QSettings().setValue("Mode", mMode);
	QSettings().setValue("Colors", mColors+2);
	QSettings().setValue("Pegs", mPegs+2);
	QSettings().setValue("Algorithm", mAlgorithm);
	QSettings().setValue("SetPins",	mSetPins);
	QSettings().setValue("CloseRow", mCloseRow);
	QSettings().setValue("Indicator", mIndicator);
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
			tr("Copyright &copy; 2013-%1 Omid Nikta").arg("2013"),
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
		pegsNumberComboBox->addItem(QString::number(i)+" Slots");
	pegsNumberComboBox->setCurrentIndex(mPegs);
	pegsNumberComboBox->setToolTip("Choose the numbe of pegs");

	connect(pegsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateNumbersSlot()));
}
//-----------------------------------------------------------------------------

void MainWindow::setColorsComboBox()
{
	colorsNumberComboBox = new QComboBox(this);
	for(int i = MIN_COLOR_NUMBER; i <= MAX_COLOR_NUMBER; ++i)
		colorsNumberComboBox->addItem(QString::number(i)+" Colors");
	colorsNumberComboBox->setCurrentIndex(mColors);
	colorsNumberComboBox->setToolTip("Choose the number of colors");

	connect(colorsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateNumbersSlot()));

}
//-----------------------------------------------------------------------------

void MainWindow::setSolvingAlgorithmsComboBox()
{
	solvingAlgorithmsComboBox = new QComboBox(this);
	solvingAlgorithmsComboBox->setToolTip("Choose the solving algorithm");
	solvingAlgorithmsComboBox->addItem("Most Parts");
	solvingAlgorithmsComboBox->addItem("Worst Case");
	solvingAlgorithmsComboBox->addItem("Expected Size");
	solvingAlgorithmsComboBox->setCurrentIndex(mAlgorithm);

	connect(solvingAlgorithmsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateNumbersSlot()));

}
//-----------------------------------------------------------------------------

void MainWindow::createMenuBar()
{
	auto gameMenu = menuBar()->addMenu(tr("&Game"));

	QIcon new_icon(QPixmap(":icons/new-icon.png"));
	auto newGameAction = gameMenu->addAction(new_icon, tr("&New"), this, SLOT(newGameSlot()), tr("Ctrl+N"));

	QIcon throwtowel_icon(QPixmap(":/icons/face-raspberry.png"));
	throwInTheTowelAction = gameMenu->addAction(throwtowel_icon, tr("&Throw In The Towel"), mBoard, SLOT(onThowInTheTowel()));
	throwInTheTowelAction->setVisible(mMode == MODE_MASTER);

	gameMenu->addSeparator();

	QIcon close_icon(QPixmap(":/icons/close-icon.png"));
	gameMenu->addAction(close_icon, tr("Quit"), this, SLOT(close()), tr("Ctrl+Q"));

	auto settingsMenu = menuBar()->addMenu(tr("&Settings"));

	auto gameModeSubMenu = settingsMenu->addMenu(tr("Game Mode"));
	auto gameModeActions = new QActionGroup(this);
	char* gameModeNames[] = {(char*)"Code Master", (char*)"Code Break"};
	for(int i = 0; i < 2; ++i){
		auto gameModeAction = gameModeSubMenu->addAction(tr(gameModeNames[i]));
		gameModeAction->setData(i);
		gameModeAction->setCheckable(true);
		gameModeAction->setChecked(mMode == i);
		gameModeActions->addAction(gameModeAction);
	}
	gameModeActions->setExclusive(true);
	connect(gameModeActions, SIGNAL(triggered(QAction*)), this, SLOT(changeGameModeSlot(QAction*)));

	auto indicatorTypeSubMenu = settingsMenu->addMenu(tr("Indicator Type"));
	auto indicatorTypeActions = new QActionGroup(this);
	char* indicatorTypeNames[3] = {(char*)"No Indicator", (char*)"Character Indicator", (char*)"Digit Indicator"};
	for(int i = 0; i < 3; ++i){
		auto indicatorAction = indicatorTypeSubMenu->addAction(tr(indicatorTypeNames[i]));
		indicatorAction->setData(i);
		indicatorAction->setCheckable(true);
		indicatorAction->setChecked(mIndicator == i);
		indicatorTypeActions->addAction(indicatorAction);
	}
	indicatorTypeActions->setExclusive(true);
	connect(indicatorTypeActions, SIGNAL(triggered(QAction*)), this, SLOT(changeIndicatorsSlot(QAction*)));

	setPinsAutomaticallyAction = settingsMenu->addAction(tr("&Set Pins Automatically"), this, SLOT(setPinsCloseRowAutomatically()), tr("Ctrl+P"));
	setPinsAutomaticallyAction->setCheckable(true);
	setPinsAutomaticallyAction->setChecked(mSetPins == 1);

	closeRowAutomaticallyAction = settingsMenu->addAction(tr("&Clost Row Automatically"), this, SLOT(setPinsCloseRowAutomatically()), tr("Ctrl+R"));
	closeRowAutomaticallyAction->setCheckable(true);
	closeRowAutomaticallyAction->setChecked(mCloseRow == 1);

	auto helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(tr("&About"), this, SLOT(about()));
	helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));

	auto mainToolbar = new QToolBar(this);
	mainToolbar->setIconSize(QSize(22, 22));
	mainToolbar->setFloatable(false);
	mainToolbar->setMovable(false);
	mainToolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
	mainToolbar->addAction(newGameAction);
//	mainToolbar->addAction(restartGameAction);
	mainToolbar->addAction(throwInTheTowelAction);

	QIcon do_icon;
	do_icon.addPixmap(QPixmap(":/icons/monkey-icon.png"));
	doItForMeAction = new QAction(do_icon, tr("&Put Pins For Me"), this);
	mainToolbar->addAction(doItForMeAction);

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
