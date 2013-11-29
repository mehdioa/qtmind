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
#include "ui_mainwindow.h"
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
	QMainWindow(parent),
	ui(new Ui::MainWindow)

{
	ui->setupUi(this);

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
	setWindowTitle(tr("Mastermind"));
	setCentralWidget(mBoard);
	connect(this, SIGNAL(changeIndicatorsSignal(IndicatorType)), mBoard, SLOT(onChangeIndicators(IndicatorType)));

	Preferences::loadTranslation("mastermind_");

	ui->menuGame->setTitle(tr("Game"));
	ui->actionNew->setText(tr("New"));
	ui->actionResign->setText(tr("Resign"));
	ui->actionQuit->setText(tr("Quit"));

	ui->menuTools->setTitle(tr("Tools"));
	ui->menuGame_Mode->setTitle(tr("Game Mode"));
	ui->actionCode_Breaker->setText(tr("Code Breaker"));
	ui->actionCode_Master->setText(tr("Code Master"));
	ui->menuIndicator_Type->setTitle(tr("Indicator Type"));
	ui->actionNo_Indicators->setText(tr("No Indicatro"));
	ui->actionCharacters->setText(tr("Character"));
	ui->actionDigits->setText(tr("Digit"));
	ui->actionAuto_Set_Pins->setText(tr("Auto Put Pins"));
	ui->actionAuto_Close_Rows->setText(tr("Auto Close Rows"));
	ui->actionOptions->setText(tr("Options"));

	ui->menuHelp->setTitle(tr("Help"));
	ui->actionAbout_Mastermind->setText(tr("About Mastermind"));
	ui->actionAbout_Qt->setText(tr("About Qt"));

	auto indicatorTypeActions = new QActionGroup(this);
	indicatorTypeActions->addAction(ui->actionNo_Indicators);
	indicatorTypeActions->addAction(ui->actionCharacters);
	indicatorTypeActions->addAction(ui->actionDigits);
	indicatorTypeActions->setExclusive(true);
	ui->actionNo_Indicators->setData((int) IndicatorType::None);
	ui->actionCharacters->setData((int) IndicatorType::Character);
	ui->actionDigits->setData((int) IndicatorType::Digit);
	ui->actionNo_Indicators->setChecked(mIndicator == IndicatorType::None);
	ui->actionCharacters->setChecked(mIndicator == IndicatorType::Character);
	ui->actionDigits->setChecked(mIndicator == IndicatorType::Digit);

	auto gameModeActions = new QActionGroup(this);
	gameModeActions->addAction(ui->actionCode_Master);
	gameModeActions->addAction(ui->actionCode_Breaker);
	gameModeActions->setExclusive(true);
	ui->actionCode_Master->setData((int) GameMode::Master);
	ui->actionCode_Breaker->setData((int) GameMode::Breaker);
	ui->actionCode_Master->setChecked(mMode == GameMode::Master);
	ui->actionCode_Breaker->setChecked(mMode == GameMode::Breaker);

	QIcon double_icon;
	double_icon.addPixmap(QPixmap(":/icons/same_color_1.png"), QIcon::Normal, QIcon::On);
	double_icon.addPixmap(QPixmap(":/icons/same_color_0.png"), QIcon::Normal, QIcon::Off);
	allowSameColorAction = new QAction(double_icon, tr("Allow Same Color"), this);
	allowSameColorAction->setCheckable(true);
	allowSameColorAction->setChecked(mSameColorAllowed);

	ui->actionAuto_Set_Pins->setChecked(mSetPins);
	ui->actionAuto_Close_Rows->setChecked(mCloseRow);

	setPegsComboBox();
	setColorsComboBox();
	setSolvingAlgorithmsComboBox();

	ui->toolBar->addAction(ui->actionNew);
	ui->toolBar->addAction(allowSameColorAction);
	ui->toolBar->addSeparator();
	ui->toolBar->addWidget(pegsNumberComboBox);
	ui->toolBar->addWidget(colorsNumberComboBox);
	ui->toolBar->addWidget(solvingAlgorithmsComboBox);

	connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(onNewGame()));
	connect(ui->actionResign, SIGNAL(triggered()), mBoard, SLOT(onResign()));
	connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(gameModeActions, SIGNAL(triggered(QAction*)), this, SLOT(onChangeGameMode(QAction*)));
	connect(indicatorTypeActions, SIGNAL(triggered(QAction*)), SLOT(onChangeIndicators(QAction*)));
	connect(allowSameColorAction, SIGNAL(triggered()), this, SLOT(onUpdateNumbers()));
	connect(ui->actionAuto_Set_Pins, SIGNAL(triggered()), this, SLOT(onSetPinsCloseRowAutomatically()));
	connect(ui->actionAuto_Close_Rows, SIGNAL(triggered()), this, SLOT(onSetPinsCloseRowAutomatically()));
	connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(onPreferences()));
	connect(ui->actionAbout_Mastermind, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(colorsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onUpdateNumbers()));
	connect(pegsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onUpdateNumbers()));
	connect(solvingAlgorithmsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onUpdateNumbers()));
	connect(this, SIGNAL(newGame()), this, SLOT(onNewGame()));
	connect(this, SIGNAL(updateNumbers()), this, SLOT(onUpdateNumbers()));

	setContextMenuPolicy(Qt::NoContextMenu);

	emit updateNumbers();

	restoreGeometry(QSettings().value("Geometry").toByteArray());
}
//-----------------------------------------------------------------------------

MainWindow::~MainWindow()
{
	delete ui;
}
//-----------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent *event)
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

void MainWindow::onNewGame()
{
	ui->actionResign->setEnabled(mMode == GameMode::Breaker);
	mBoard->reset(mPegs+2, mColors+2, mMode, mSameColorAllowed, mAlgorithm, mSetPins, mCloseRow, mLocale, mIndicator);
	mBoard->play(mMode);
}
//-----------------------------------------------------------------------------

void MainWindow::onPreferences()
{
	auto preferencesWidget = new Preferences(&mLocale, this);
	preferencesWidget->setWindowTitle(tr("Options"));
	preferencesWidget->exec();
}
//-----------------------------------------------------------------------------

void MainWindow::onSetPinsCloseRowAutomatically()
{
	mSetPins = ui->actionAuto_Set_Pins->isChecked();
	mCloseRow = ui->actionAuto_Close_Rows->isChecked();
	mBoard->setPinsRow(mSetPins, mCloseRow);
}
//-----------------------------------------------------------------------------

void MainWindow::onChangeIndicators(QAction *selectedAction)
{
	mIndicator = (IndicatorType) selectedAction->data().toInt();
	emit changeIndicatorsSignal(mIndicator);
}
//-----------------------------------------------------------------------------

void MainWindow::onChangeGameMode(QAction *selectedAction)
{
	mMode =  (GameMode) selectedAction->data().toInt();
	emit updateNumbers();
}
//-----------------------------------------------------------------------------

void MainWindow::onUpdateNumbers()
{
	mColors = colorsNumberComboBox->currentIndex();
	mPegs = pegsNumberComboBox->currentIndex();
	mAlgorithm = (Algorithm) solvingAlgorithmsComboBox->currentIndex();
	mSameColorAllowed = (allowSameColorAction->isChecked()) || (mPegs > mColors);
	allowSameColorAction->setChecked(mSameColorAllowed);

	// for safety, fallback to standard in out-range inputs
	if (mColors < MIN_SLOT_NUMBER || mPegs > MAX_SLOT_NUMBER ||
			mColors < MIN_COLOR_NUMBER || mPegs > MAX_COLOR_NUMBER)
	{
		mPegs = 4;
		mColors = 6;
	}

	if(mBoard)
	{
		mBoard->setAlgorithm(mAlgorithm);
		if(mBoard->getState() == GameState::Lose ||
				mBoard->getState() == GameState::Win ||
				mBoard->getState() == GameState::None ||
				mBoard->getState() == GameState::WaittingDoneButtonPress ||
				mBoard->getState() == GameState::WaittingFirstRowFill)
		{
			emit newGame();
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

void MainWindow::onAbout()
{
	QMessageBox::about(this, tr("About Mastermind"), QString(
		"<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>"
		"<p align='center'>%6<br/><small>%7</small></p>")
		.arg(tr("Mastermind"), QCoreApplication::applicationVersion(),
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
}
