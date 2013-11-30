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
	mColors = QSettings().value("Colors", 6).toInt();
	mPegs = QSettings().value("Pegs", 4).toInt();
	mAlgorithm = (Algorithm) QSettings().value("Algorithm", 0).toInt();
	mSameColorAllowed = QSettings().value("SameColor", true).toBool();
	mAutoPutPins = QSettings().value("SetPins", true).toBool();
	mAutoCloseRow = QSettings().value("CloseRow", true).toBool();
	mIndicator = (IndicatorType) QSettings().value("Indicator", 0).toInt();
	mLocale = QLocale(QSettings().value("Locale/Language", "en").toString().left(5));
	mFontName = QSettings().value("FontName", "SansSerif").toString();
	mFontSize = QSettings().value("FontSize", 12).toInt();

	mLocale.setNumberOptions(QLocale::OmitGroupSeparator);

	mBoard = new Board(mFontName, mFontSize, this);

	setLayoutDirection(mLocale.textDirection());
	setWindowTitle(tr("QtMind"));
	setCentralWidget(mBoard);
	connect(this, SIGNAL(changeIndicatorsSignal(IndicatorType)), mBoard, SLOT(onChangeIndicatorType(IndicatorType)));

	Preferences::loadTranslation("qtmind_");

	ui->menuGame->setTitle(tr("Game"));
	ui->actionNew->setText(tr("New"));
	ui->actionResign->setText(tr("Resign"));
	ui->actionQuit->setText(tr("Quit"));

	ui->menuTools->setTitle(tr("Tools"));
	ui->menuGame_Mode->setTitle(tr("Game Mode"));
	ui->actionCode_Breaker->setText(tr("Code Breaker"));
	ui->actionCode_Master->setText(tr("Code Master"));
	ui->menuIndicator_Type->setTitle(tr("Indicator Type"));
	ui->actionNo_Indicators->setText(tr("No Indicator"));
	ui->actionCharacters->setText(tr("Character"));
	ui->actionDigits->setText(tr("Digit"));
	ui->actionAuto_Set_Pins->setText(tr("Auto Put Pins"));
	ui->actionAuto_Close_Rows->setText(tr("Auto Close Rows"));
	ui->actionOptions->setText(tr("Options"));

	ui->menuHelp->setTitle(tr("Help"));
	ui->actionAbout_QtMind->setText(tr("About QtMind"));
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

	ui->actionAuto_Set_Pins->setChecked(mAutoPutPins);
	ui->actionAuto_Close_Rows->setChecked(mAutoCloseRow);

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
	connect(ui->actionAbout_QtMind, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(colorsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onUpdateNumbers()));
	connect(pegsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onUpdateNumbers()));
	connect(solvingAlgorithmsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onUpdateNumbers()));

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onShowContextMenu(QPoint)));

	onUpdateNumbers();

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
	QSettings().setValue("Colors", mColors);
	QSettings().setValue("Pegs", mPegs);
	QSettings().setValue("Algorithm", (int) mAlgorithm);
	QSettings().setValue("SameColor", mSameColorAllowed);
	QSettings().setValue("SetPins",	mAutoPutPins);
	QSettings().setValue("CloseRow", mAutoCloseRow);
	QSettings().setValue("Indicator", (int) mIndicator);
	QSettings().setValue("Geometry", saveGeometry());
	QMainWindow::closeEvent(event);
}
//-----------------------------------------------------------------------------

void MainWindow::onNewGame()
{
	ui->actionResign->setEnabled(mMode == GameMode::Breaker);
	mBoard->reset(mPegs, mColors, mMode, mSameColorAllowed, mAlgorithm,
				  mAutoPutPins, mAutoCloseRow, mLocale, mIndicator);

	if(mBoard->getState() != GameState::None &&
			mBoard->getState() != GameState::Win &&
			mBoard->getState() != GameState::Lose &&
			mBoard->getState() != GameState::WaittingFirstRowFill)
	{
		int new_game_accept = QMessageBox::warning(this,
									  tr("New Game"), QString("<p align='center'>%1</p>"
														"<p align='center'>%2</p>")
													.arg(tr("An unfinished game is in progress."))
													.arg(tr("Do you want to start a new game?")),
									  QMessageBox::Yes | QMessageBox::No);
		if (new_game_accept == QMessageBox::Yes)
		{
			mBoard->play();
		}
	}
	else
		mBoard->play();
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
	mAutoPutPins = ui->actionAuto_Set_Pins->isChecked();
	mAutoCloseRow = ui->actionAuto_Close_Rows->isChecked();
	mBoard->autoPutPinsCloseRow(mAutoPutPins, mAutoCloseRow);
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
	onUpdateNumbers();
}
//-----------------------------------------------------------------------------

void MainWindow::onUpdateNumbers()
{
	mColors = colorsNumberComboBox->currentIndex() + MIN_COLOR_NUMBER;
	mPegs = pegsNumberComboBox->currentIndex() + MIN_SLOT_NUMBER;
	mAlgorithm = (Algorithm) solvingAlgorithmsComboBox->currentIndex();
	mSameColorAllowed = (allowSameColorAction->isChecked()) || (mPegs > mColors);
	allowSameColorAction->setChecked(mSameColorAllowed);

	// for safety, fallback to standard in out-range inputs
	if (mPegs < MIN_SLOT_NUMBER || mPegs > MAX_SLOT_NUMBER ||
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
				mBoard->getState() == GameState::WaittingFirstRowFill)
		{
			onNewGame();
		}
	}
}
//-----------------------------------------------------------------------------

void MainWindow::onAbout()
{
	QMessageBox::about(this, tr("About QtMind"), QString(
		"<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>"
		"<p align='center'>%6<br/><small>%7</small></p>")
		.arg(tr("QtMind"), QCoreApplication::applicationVersion(),
			tr("Code Breaking Game, A Clone Of The Mastermind Board Game"),
			tr("Copyright &copy; 2013-%1 Omid Nikta").arg(mLocale.toString(2013)),
			tr("Released under the <a href=%1>GPL 3</a> license").arg("\"http://www.gnu.org/licenses/gpl.html\""),
			tr("Uses icons from the <a href=%1>Oxygen</a> icon theme").arg("\"http://www.oxygen-icons.org/\""),
			tr("Used under the <a href=%1>LGPL 3</a> license").arg("\"http://www.gnu.org/licenses/lgpl.html\""))
					   );
}

void MainWindow::onShowContextMenu(const QPoint &position)
{
	QMenu contextMenu(this);
	contextMenu.addAction(ui->actionNew);
	contextMenu.addAction(ui->actionResign);
	contextMenu.addSeparator();
	contextMenu.addMenu(ui->menuGame_Mode);
	contextMenu.exec(mapToGlobal(position));
}
//-----------------------------------------------------------------------------

void MainWindow::setPegsComboBox()
{
	pegsNumberComboBox = new QComboBox(this);
	for(int i = MIN_SLOT_NUMBER; i <= MAX_SLOT_NUMBER; ++i)
	{
		pegsNumberComboBox->addItem(QString("%1 %2").arg(mLocale.toString(i)).arg(tr("Slots")));
	}
	pegsNumberComboBox->setCurrentIndex(mPegs-MIN_SLOT_NUMBER);
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
	colorsNumberComboBox->setCurrentIndex(mColors-MIN_COLOR_NUMBER);
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
