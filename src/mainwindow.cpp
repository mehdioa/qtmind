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
	gameMode((GameMode) QSettings().value("Mode", 0).toInt()),
	colorNumber(QSettings().value("Colors", 6).toInt()),
	pegNumber(QSettings().value("Pegs", 4).toInt()),
	algorithm((Algorithm) QSettings().value("Algorithm", 0).toInt()),
	showColors(QSettings().value("ShowColors", 1).toBool()),
	showIndicators(QSettings().value("ShowIndicators", 0).toBool()),
	indicatorType((IndicatorType) QSettings().value("IndicatorType", 0).toInt()),
	sameColorAllowed(QSettings().value("SameColor", true).toBool()),
	autoPutPins(QSettings().value("SetPins", true).toBool()),
	autoCloseRows(QSettings().value("AutoCloseRows", true).toBool()),
	locale(QLocale(QSettings().value("Locale/Language", "en").toString().left(5))),
	volume(QSettings().value("Volume", 70).toInt())
{
	ui->setupUi(this);

	locale.setNumberOptions(QLocale::OmitGroupSeparator);
	Preferences::loadTranslation("qtmind_");

	game = new Game(this);

	setLayoutDirection(locale.textDirection());
	setWindowTitle(tr("QtMind"));
	setCentralWidget(game);
	connect(this, SIGNAL(showIndicatorsSignal(bool,bool,IndicatorType)), game, SLOT(onShowIndicators(bool,bool,IndicatorType)));
	connect(this, SIGNAL(preferencesChangeSignal()), game, SLOT(onPreferencesChanged()));
	emit preferencesChangeSignal();
	emit showIndicatorsSignal(showColors, showIndicators, indicatorType);


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
	ui->actionQtMind_Home_Page->setText(tr("QtMind &Home Page"));
	ui->actionAbout_QtMind->setText(tr("About Qt&Mind"));
	ui->actionAbout_Qt->setText(tr("About &Qt"));

	auto gameModeActions = new QActionGroup(this);
	gameModeActions->addAction(ui->actionCodemaker);
	gameModeActions->addAction(ui->actionCodebreaker);
	gameModeActions->setExclusive(true);
	ui->actionCodemaker->setData((int) GameMode::Codemaker);
	ui->actionCodebreaker->setData((int) GameMode::Codebreaker);
	ui->actionCodemaker->setChecked(gameMode == GameMode::Codemaker);
	ui->actionCodebreaker->setChecked(gameMode == GameMode::Codebreaker);

	QIcon double_icon;
	double_icon.addPixmap(QPixmap("://icons/same_color_1.png"), QIcon::Normal, QIcon::On);
	double_icon.addPixmap(QPixmap("://icons/same_color_0.png"), QIcon::Normal, QIcon::Off);
	allowSameColorAction = new QAction(double_icon, tr("Allow Same Color"), this);
	allowSameColorAction->setCheckable(true);
	allowSameColorAction->setChecked(sameColorAllowed);

	ui->actionShow_Indicators->setChecked(showIndicators);
	ui->actionAuto_Set_Pins->setChecked(autoPutPins);
	ui->actionAuto_Close_Rows->setChecked(autoCloseRows);

	pegsNumberComboBox = new QComboBox(this);
	for(int i = MIN_SLOT_NUMBER; i <= MAX_SLOT_NUMBER; ++i)
	{
		pegsNumberComboBox->addItem(QString("%1 %2").arg(locale.toString(i), tr("Slots")));
	}
	pegsNumberComboBox->setCurrentIndex(pegNumber-MIN_SLOT_NUMBER);
	pegsNumberComboBox->setToolTip(tr("Choose the numbe of slots"));
	pegsNumberComboBox->setLocale(locale);

	colorsNumberComboBox = new QComboBox(this);
	for(int i = MIN_COLOR_NUMBER; i <= MAX_COLOR_NUMBER; ++i)
	{
		colorsNumberComboBox->addItem(QString("%1 %2").arg(locale.toString(i), tr("Colors")));
	}
	colorsNumberComboBox->setCurrentIndex(colorNumber-MIN_COLOR_NUMBER);
	colorsNumberComboBox->setToolTip(tr("Choose the number of colors"));


	solvingAlgorithmsComboBox = new QComboBox(this);
	solvingAlgorithmsComboBox->setToolTip(tr("Choose the solving algorithm"));
	solvingAlgorithmsComboBox->addItem(tr("Most Parts"));
	solvingAlgorithmsComboBox->addItem(tr("Worst Case"));
	solvingAlgorithmsComboBox->addItem(tr("Expected Size"));
	solvingAlgorithmsComboBox->setCurrentIndex((int) algorithm);


	ui->toolBar->addAction(ui->actionNew);
	ui->toolBar->addAction(allowSameColorAction);
	ui->toolBar->addSeparator();
	ui->toolBar->addWidget(pegsNumberComboBox);
	ui->toolBar->addWidget(colorsNumberComboBox);
	ui->toolBar->addWidget(solvingAlgorithmsComboBox);

	connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(onNewGame()));
	connect(ui->actionReveal_One_Peg, SIGNAL(triggered()), game, SLOT(onRevealOnePeg()));
	connect(ui->actionResign, SIGNAL(triggered()), game, SLOT(onResigned()));
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
	QSettings().setValue("Mode", (int) gameMode);
	QSettings().setValue("Colors", colorNumber);
	QSettings().setValue("Pegs", pegNumber);
	QSettings().setValue("Algorithm", (int) algorithm);
	QSettings().setValue("SameColor", sameColorAllowed);
	QSettings().setValue("SetPins",	autoPutPins);
	QSettings().setValue("AutoCloseRows", autoCloseRows);
	QSettings().setValue("ShowColors", (int) showColors);
	QSettings().setValue("ShowIndicators", (int) showIndicators);
	QSettings().setValue("IndicatorType", (int) indicatorType);
	QSettings().setValue("Geometry", saveGeometry());
	QMainWindow::closeEvent(event);
}
//-----------------------------------------------------------------------------

void MainWindow::onNewGame()
{
	ui->actionResign->setEnabled(gameMode == GameMode::Codebreaker);
	ui->actionResign->setVisible(gameMode == GameMode::Codebreaker);
	ui->actionReveal_One_Peg->setEnabled(gameMode == GameMode::Codebreaker);
	ui->actionReveal_One_Peg->setVisible(gameMode == GameMode::Codebreaker);

	game->reset(pegNumber, colorNumber, gameMode, sameColorAllowed, algorithm,
				  autoPutPins, autoCloseRows, &locale, showColors, showIndicators, indicatorType);

	if(game->getState() != GameState::None &&
			game->getState() != GameState::Win &&
			game->getState() != GameState::Lose &&
			game->getState() != GameState::WaittingFirstRowFill)
	{
		int new_game_accept = QMessageBox::warning(this,
									  tr("New Game"), QString("<p align='center'>%1</p>"
														"<p align='center'>%2</p>")
													.arg(tr("An unfinished game is in progress."))
													.arg(tr("Do you want to start a new game?")),
									  QMessageBox::Yes | QMessageBox::No);
		if (new_game_accept == QMessageBox::Yes)
		{
			game->play();
		}
	}
	else
		game->play();
}
//-----------------------------------------------------------------------------

void MainWindow::onPreferences()
{
	auto preferencesWidget = new Preferences(&locale, this);
	preferencesWidget->setWindowTitle(tr("Options"));
	preferencesWidget->exec();
	volume = QSettings().value("Volume", 70).toInt();
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
	autoPutPins = ui->actionAuto_Set_Pins->isChecked();
	autoCloseRows = ui->actionAuto_Close_Rows->isChecked();
	game->autoPutPinsCloseRow(autoPutPins, autoCloseRows);
}
//-----------------------------------------------------------------------------

void MainWindow::onIndicatorChanged()
{
	bool newShowIndicators = ui->actionShow_Indicators->isChecked();
	bool newShowColors = (bool) QSettings().value("ShowColors", 1).toBool();
	IndicatorType newIndicatorType = (IndicatorType) QSettings().value("IndicatorType", 0).toInt();
	if (showColors == newShowColors &&
			showIndicators == newShowIndicators &&
			indicatorType == newIndicatorType)
		return;
	showColors = newShowColors;
	showIndicators = newShowIndicators;
	indicatorType = newIndicatorType;
	emit showIndicatorsSignal(showColors, showIndicators, indicatorType);
}
//-----------------------------------------------------------------------------

void MainWindow::onGameModeChanged(QAction *selectedAction)
{
	gameMode =  (GameMode) selectedAction->data().toInt();
	onUpdateNumbers();
}
//-----------------------------------------------------------------------------

void MainWindow::onUpdateNumbers()
{
	colorNumber = colorsNumberComboBox->currentIndex() + MIN_COLOR_NUMBER;
	pegNumber = pegsNumberComboBox->currentIndex() + MIN_SLOT_NUMBER;
	algorithm = (Algorithm) solvingAlgorithmsComboBox->currentIndex();
	sameColorAllowed = (allowSameColorAction->isChecked()) || (pegNumber > colorNumber);
	allowSameColorAction->setChecked(sameColorAllowed);
	if (sameColorAllowed)
		allowSameColorAction->setToolTip(tr("Same Color Allowed"));
	else
		allowSameColorAction->setToolTip(tr("Same Color Not Allwed"));

	// for safety, fallback to standard in out-range inputs
	if (pegNumber < MIN_SLOT_NUMBER || pegNumber > MAX_SLOT_NUMBER ||
			colorNumber < MIN_COLOR_NUMBER || pegNumber > MAX_COLOR_NUMBER)
	{
		pegNumber = 4;
		colorNumber = 6;
	}

	if(game)
	{
		game->setAlgorithm(algorithm);
		if(game->getState() == GameState::Lose ||
				game->getState() == GameState::Win ||
				game->getState() == GameState::None ||
				game->getState() == GameState::WaittingFirstRowFill)
		{
			onNewGame();
		}
	}
}
//-----------------------------------------------------------------------------

void MainWindow::onAbout()
{
	QStringList app_version = QCoreApplication::applicationVersion().split('.');
	QString localized_app_version = "";
	foreach (QString sub_version_number, app_version) {
		localized_app_version.append(locale.toString(sub_version_number.toInt()));
		localized_app_version.append(locale.decimalPoint());
	}
	localized_app_version.chop(1);
	QMessageBox::about(this, tr("About QtMind"), QString(
		"<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>")
		.arg(tr("QtMind"), localized_app_version,
			tr("Code Breaking Game, A Clone Of The Mastermind Board Game"),
			tr("Copyright &copy; 2013-%1 Omid Nikta").arg(locale.toString(2013)),
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
