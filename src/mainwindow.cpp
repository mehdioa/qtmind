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
#include <QComboBox>
#include <QMessageBox>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	mGameMode((GameMode) QSettings().value("Mode", 0).toInt()),
	mColorNumber(QSettings().value("Colors", 6).toInt()),
	mPegNumber(QSettings().value("Pegs", 4).toInt()),
	mAlgorithm((Algorithm) QSettings().value("Algorithm", 0).toInt()),
	mShowColors(QSettings().value("ShowColors", 1).toBool()),
	mShowIndicators(QSettings().value("ShowIndicators", 0).toBool()),
	mIndicatorType((IndicatorType) QSettings().value("IndicatorType", 0).toInt()),
	mSameColorAllowed(QSettings().value("SameColor", true).toBool()),
	mAutoPutPins(QSettings().value("SetPins", true).toBool()),
	mAutoCloseRows(QSettings().value("AutoCloseRows", true).toBool()),
	mLocale(QLocale(QSettings().value("Locale/Language", "en").toString().left(5))),
	mVolume(QSettings().value("Volume", 70).toInt())
{
	ui->setupUi(this);

	mLocale.setNumberOptions(QLocale::OmitGroupSeparator);
	Preferences::loadTranslation("qtmind_");

	mBoard = new Board(this);

	setLayoutDirection(mLocale.textDirection());
	setWindowTitle(tr("QtMind"));
	setCentralWidget(mBoard);
	connect(this, SIGNAL(showIndicatorsSignal(bool,bool,IndicatorType)), mBoard, SLOT(onShowIndicators(bool,bool,IndicatorType)));
	connect(this, SIGNAL(preferencesChangeSignal()), mBoard, SLOT(onPreferencesChanged()));
	emit preferencesChangeSignal();
	emit showIndicatorsSignal(mShowColors, mShowIndicators, mIndicatorType);


	ui->menuGame->setTitle(tr("&Game"));
	ui->actionNew->setText(tr("&New"));
	ui->actionReveal_One_Peg->setText(tr("Reveal One &Peg"));
	ui->actionResign->setText(tr("&Resign"));
	ui->actionQuit->setText(tr("&Quit"));

	ui->menuTools->setTitle(tr("&Tools"));
	ui->menuGame_Mode->setTitle(tr("Game &Mode"));
	ui->actionCodebreaker->setText(tr("Code&breaker"));
	ui->actionCodemaker->setText(tr("Code&maker"));
	ui->actionShow_Indicators->setText(tr("Show &Indicators"));
	ui->actionAuto_Set_Pins->setText(tr("Auto Put &Pins"));
	ui->actionAuto_Close_Rows->setText(tr("Auto Close &Rows"));
	ui->actionOptions->setText(tr("&Options"));

	ui->menuHelp->setTitle(tr("&Help"));
	ui->actionAbout_QtMind->setText(tr("About Qt&Mind"));
	ui->actionAbout_Qt->setText(tr("About &Qt"));

	auto gameModeActions = new QActionGroup(this);
	gameModeActions->addAction(ui->actionCodemaker);
	gameModeActions->addAction(ui->actionCodebreaker);
	gameModeActions->setExclusive(true);
	ui->actionCodemaker->setData((int) GameMode::Codemaker);
	ui->actionCodebreaker->setData((int) GameMode::Codebreaker);
	ui->actionCodemaker->setChecked(mGameMode == GameMode::Codemaker);
	ui->actionCodebreaker->setChecked(mGameMode == GameMode::Codebreaker);

	QIcon double_icon;
	double_icon.addPixmap(QPixmap("://icons/same_color_1.png"), QIcon::Normal, QIcon::On);
	double_icon.addPixmap(QPixmap("://icons/same_color_0.png"), QIcon::Normal, QIcon::Off);
	allowSameColorAction = new QAction(double_icon, tr("Allow Same Color"), this);
	allowSameColorAction->setCheckable(true);
	allowSameColorAction->setChecked(mSameColorAllowed);

	ui->actionShow_Indicators->setChecked(mShowIndicators);
	ui->actionAuto_Set_Pins->setChecked(mAutoPutPins);
	ui->actionAuto_Close_Rows->setChecked(mAutoCloseRows);

	pegsNumberComboBox = new QComboBox(this);
	for(int i = MIN_SLOT_NUMBER; i <= MAX_SLOT_NUMBER; ++i)
	{
		pegsNumberComboBox->addItem(QString("%1 %2").arg(mLocale.toString(i)).arg(tr("Slots")));
	}
	pegsNumberComboBox->setCurrentIndex(mPegNumber-MIN_SLOT_NUMBER);
	pegsNumberComboBox->setToolTip(tr("Choose the numbe of slots"));
	pegsNumberComboBox->setLocale(mLocale);

	colorsNumberComboBox = new QComboBox(this);
	for(int i = MIN_COLOR_NUMBER; i <= MAX_COLOR_NUMBER; ++i)
	{
		colorsNumberComboBox->addItem(QString("%1 %2").arg(mLocale.toString(i)).arg(tr("Colors")));
	}
	colorsNumberComboBox->setCurrentIndex(mColorNumber-MIN_COLOR_NUMBER);
	colorsNumberComboBox->setToolTip(tr("Choose the number of colors"));


	solvingAlgorithmsComboBox = new QComboBox(this);
	solvingAlgorithmsComboBox->setToolTip(tr("Choose the solving algorithm"));
	solvingAlgorithmsComboBox->addItem(tr("Most Parts"));
	solvingAlgorithmsComboBox->addItem(tr("Worst Case"));
	solvingAlgorithmsComboBox->addItem(tr("Expected Size"));
	solvingAlgorithmsComboBox->setCurrentIndex((int) mAlgorithm);


	ui->toolBar->addAction(ui->actionNew);
	ui->toolBar->addAction(allowSameColorAction);
	ui->toolBar->addSeparator();
	ui->toolBar->addWidget(pegsNumberComboBox);
	ui->toolBar->addWidget(colorsNumberComboBox);
	ui->toolBar->addWidget(solvingAlgorithmsComboBox);

	connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(onNewGame()));
	connect(ui->actionReveal_One_Peg, SIGNAL(triggered()), mBoard, SLOT(onRevealOnePeg()));
	connect(ui->actionResign, SIGNAL(triggered()), mBoard, SLOT(onResigned()));
	connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(gameModeActions, SIGNAL(triggered(QAction*)), this, SLOT(onGameModeChanged(QAction*)));
	connect(ui->actionShow_Indicators, SIGNAL(triggered()), SLOT(onIndicatorChanged()));
	connect(allowSameColorAction, SIGNAL(triggered()), this, SLOT(onUpdateNumbers()));
	connect(ui->actionAuto_Set_Pins, SIGNAL(triggered()), this, SLOT(onSetPinsCloseRowAutomatically()));
	connect(ui->actionAuto_Close_Rows, SIGNAL(triggered()), this, SLOT(onSetPinsCloseRowAutomatically()));
	connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(onPreferences()));
	connect(ui->actionQtMind_Home_Page, SIGNAL(triggered()), this, SLOT(onQtMindHomePage()));
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
	QSettings().setValue("Mode", (int) mGameMode);
	QSettings().setValue("Colors", mColorNumber);
	QSettings().setValue("Pegs", mPegNumber);
	QSettings().setValue("Algorithm", (int) mAlgorithm);
	QSettings().setValue("SameColor", mSameColorAllowed);
	QSettings().setValue("SetPins",	mAutoPutPins);
	QSettings().setValue("AutoCloseRows", mAutoCloseRows);
	QSettings().setValue("ShowColors", (int) mShowColors);
	QSettings().setValue("ShowIndicators", (int) mShowIndicators);
	QSettings().setValue("IndicatorType", (int) mIndicatorType);
	QSettings().setValue("Geometry", saveGeometry());
	QMainWindow::closeEvent(event);
}
//-----------------------------------------------------------------------------

void MainWindow::onNewGame()
{
	ui->actionResign->setEnabled(mGameMode == GameMode::Codebreaker);
	ui->actionResign->setVisible(mGameMode == GameMode::Codebreaker);
	ui->actionReveal_One_Peg->setEnabled(mGameMode == GameMode::Codebreaker);
	ui->actionReveal_One_Peg->setVisible(mGameMode == GameMode::Codebreaker);

	mBoard->reset(mPegNumber, mColorNumber, mGameMode, mSameColorAllowed, mAlgorithm,
				  mAutoPutPins, mAutoCloseRows, &mLocale, mShowColors, mShowIndicators, mIndicatorType);

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
	mVolume = QSettings().value("Volume", 70).toInt();
	emit preferencesChangeSignal();
	onIndicatorChanged();
}
//-----------------------------------------------------------------------------

void MainWindow::onQtMindHomePage()
{
	QDesktopServices::openUrl(QUrl("http://omidnikta.github.io/qtmind/index.html", QUrl::TolerantMode));

}
//-----------------------------------------------------------------------------

void MainWindow::onSetPinsCloseRowAutomatically()
{
	mAutoPutPins = ui->actionAuto_Set_Pins->isChecked();
	mAutoCloseRows = ui->actionAuto_Close_Rows->isChecked();
	mBoard->autoPutPinsCloseRow(mAutoPutPins, mAutoCloseRows);
}
//-----------------------------------------------------------------------------

void MainWindow::onIndicatorChanged()
{
	bool newShowIndicators = ui->actionShow_Indicators->isChecked();
	bool newShowColors = (bool) QSettings().value("ShowColors", 1).toBool();
	IndicatorType newIndicatorType = (IndicatorType) QSettings().value("IndicatorType", 0).toInt();
	if (mShowColors == newShowColors &&
			mShowIndicators == newShowIndicators &&
			mIndicatorType == newIndicatorType)
		return;
	mShowColors = newShowColors;
	mShowIndicators = newShowIndicators;
	mIndicatorType = newIndicatorType;
	emit showIndicatorsSignal(mShowColors, mShowIndicators, mIndicatorType);
}
//-----------------------------------------------------------------------------

void MainWindow::onGameModeChanged(QAction *selectedAction)
{
	mGameMode =  (GameMode) selectedAction->data().toInt();
	onUpdateNumbers();
}
//-----------------------------------------------------------------------------

void MainWindow::onUpdateNumbers()
{
	mColorNumber = colorsNumberComboBox->currentIndex() + MIN_COLOR_NUMBER;
	mPegNumber = pegsNumberComboBox->currentIndex() + MIN_SLOT_NUMBER;
	mAlgorithm = (Algorithm) solvingAlgorithmsComboBox->currentIndex();
	mSameColorAllowed = (allowSameColorAction->isChecked()) || (mPegNumber > mColorNumber);
	allowSameColorAction->setChecked(mSameColorAllowed);
	if (mSameColorAllowed)
		allowSameColorAction->setToolTip(tr("Same Color Allowed"));
	else
		allowSameColorAction->setToolTip(tr("Same Color Not Allwed"));

	// for safety, fallback to standard in out-range inputs
	if (mPegNumber < MIN_SLOT_NUMBER || mPegNumber > MAX_SLOT_NUMBER ||
			mColorNumber < MIN_COLOR_NUMBER || mPegNumber > MAX_COLOR_NUMBER)
	{
		mPegNumber = 4;
		mColorNumber = 6;
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
		"<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>")
		.arg(tr("QtMind"), QCoreApplication::applicationVersion(),
			tr("Code Breaking Game, A Clone Of The Mastermind Board Game"),
			tr("Copyright &copy; 2013-%1 Omid Nikta").arg(mLocale.toString(2013)),
			tr("Released under the <a href=%1>GPL 3</a> license").arg("\"http://www.gnu.org/licenses/gpl.html\"")));
}
//-----------------------------------------------------------------------------

void MainWindow::onShowContextMenu(const QPoint &position)
{
	QMenu contextMenu(this);
	contextMenu.addAction(ui->actionNew);
	contextMenu.addAction(ui->actionReveal_One_Peg);
	contextMenu.addAction(ui->actionResign);
	contextMenu.addSeparator();
	contextMenu.addMenu(ui->menuGame_Mode);
	contextMenu.exec(mapToGlobal(position));
}
