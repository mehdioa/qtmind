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
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	boardAid.locale.setNumberOptions(QLocale::OmitGroupSeparator);
	Preferences::loadTranslation(&boardAid);

	game = new Game(&gameRules, &boardAid, this);

	setLayoutDirection(boardAid.locale.textDirection());
	setCentralWidget(game);
	game->changeIndicators();



	auto gameModeActions = new QActionGroup(this);
	gameModeActions->addAction(ui->actionGameMode_MVH);
	gameModeActions->addAction(ui->actionGameMode_HVM);
	gameModeActions->setExclusive(true);
	ui->actionGameMode_MVH->setData((int) GameMode::MVH);
	ui->actionGameMode_HVM->setData((int) GameMode::HVM);
	ui->actionGameMode_MVH->setChecked(gameRules.gameMode == GameMode::MVH);
	ui->actionGameMode_HVM->setChecked(gameRules.gameMode == GameMode::HVM);

	QIcon double_icon;
	double_icon.addPixmap(QPixmap("://icons/same_color_1.png"), QIcon::Normal, QIcon::On);
	double_icon.addPixmap(QPixmap("://icons/same_color_0.png"), QIcon::Normal, QIcon::Off);
	allowSameColorAction = new QAction(double_icon, tr("Allow Same Color"), this);
	allowSameColorAction->setCheckable(true);
	allowSameColorAction->setChecked(gameRules.sameColorAllowed);

	ui->actionShow_Indicators->setChecked(boardAid.indicator.getShowIndicators());
	ui->actionAuto_Set_Pins->setChecked(boardAid.autoPutPins);
	ui->actionAuto_Close_Rows->setChecked(boardAid.autoCloseRows);

	pegsNumberComboBox = new QComboBox(this);
	for(int i = MIN_SLOT_NUMBER; i <= MAX_SLOT_NUMBER; ++i)
	{
		pegsNumberComboBox->addItem(QString("%1 %2").arg(boardAid.locale.toString(i), tr("Slot(s)", "", i)));
	}
	pegsNumberComboBox->setCurrentIndex(gameRules.pegNumber-MIN_SLOT_NUMBER);
	pegsNumberComboBox->setToolTip(tr("Choose the numbe of slots"));
	pegsNumberComboBox->setLocale(boardAid.locale);

	colorsNumberComboBox = new QComboBox(this);
	for(int i = MIN_COLOR_NUMBER; i <= MAX_COLOR_NUMBER; ++i)
	{
		colorsNumberComboBox->addItem(QString("%1 %2").arg(boardAid.locale.toString(i), tr("Color(s)", "", i)));
	}
	colorsNumberComboBox->setCurrentIndex(gameRules.colorNumber-MIN_COLOR_NUMBER);
	colorsNumberComboBox->setToolTip(tr("Choose the number of colors"));


	solvingAlgorithmsComboBox = new QComboBox(this);
	solvingAlgorithmsComboBox->setToolTip(tr("Choose the solving algorithm"));
	solvingAlgorithmsComboBox->addItem(tr("Most Parts"));
	solvingAlgorithmsComboBox->addItem(tr("Worst Case"));
	solvingAlgorithmsComboBox->addItem(tr("Expected Size"));
	solvingAlgorithmsComboBox->setCurrentIndex((int) gameRules.algorithm);


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
	connect(ui->actionShow_Indicators, SIGNAL(triggered()), this, SLOT(onIndicatorChanged()));
	connect(allowSameColorAction, SIGNAL(triggered()), this, SLOT(onNewGame()));
	connect(ui->actionAuto_Set_Pins, SIGNAL(triggered()), this, SLOT(onAutoPutPins()));
	connect(ui->actionAuto_Close_Rows, SIGNAL(triggered()), this, SLOT(onAutoCloseRows()));
	connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(onPreferences()));
	connect(ui->actionQtMind_Home_Page, SIGNAL(triggered()), this, SLOT(onQtMindHomePage()));
	connect(ui->actionAbout_QtMind, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(colorsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onNewGame()));
	connect(pegsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onNewGame()));
	connect(solvingAlgorithmsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAlgorithmChanded()));

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onShowContextMenu(QPoint)));
	onNewGame();
	resetActionsText();
	restoreGeometry(QSettings().value("Geometry").toByteArray());
}
//-----------------------------------------------------------------------------

MainWindow::~MainWindow()
{
	delete ui;
}
//-----------------------------------------------------------------------------

void MainWindow::resetActionsText()
{
	setWindowTitle(tr("QtMind"));
	ui->menuGame->setTitle(tr("&Game"));
	ui->actionNew->setText(tr("&New"));
	ui->actionReveal_One_Peg->setText(tr("Reveal One &Peg"));
	ui->actionResign->setText(tr("&Resign"));
	ui->actionQuit->setText(tr("&Quit"));

	ui->menuTools->setTitle(tr("&Tools"));
	ui->menuGameMode->setTitle(tr("Game &Mode"));
	ui->actionGameMode_MVH->setText(tr("&Machine vs Human"));
	ui->actionGameMode_HVM->setText(tr("&Human vs Machine"));
	ui->actionShow_Indicators->setText(tr("Show &Indicators"));
	ui->actionAuto_Set_Pins->setText(tr("Auto Put &Pins"));
	ui->actionAuto_Close_Rows->setText(tr("Auto Close &Rows"));
	ui->actionOptions->setText(tr("&Options"));

	ui->menuHelp->setTitle(tr("&Help"));
	ui->actionQtMind_Home_Page->setText(tr("QtMind &Home Page"));
	ui->actionAbout_QtMind->setText(tr("About Qt&Mind"));
	ui->actionAbout_Qt->setText(tr("About &Qt"));


	for(int i = MIN_SLOT_NUMBER; i <= MAX_SLOT_NUMBER; ++i)
	{
		pegsNumberComboBox->setItemText(i - MIN_SLOT_NUMBER, QString("%1 %2").arg(boardAid.locale.toString(i), tr("Slot(s)", "", i)));
	}
	pegsNumberComboBox->setToolTip(tr("Choose the numbe of slots"));

	for(int i = MIN_COLOR_NUMBER; i <= MAX_COLOR_NUMBER; ++i)
	{
		colorsNumberComboBox->setItemText(i - MIN_COLOR_NUMBER, QString("%1 %2").arg(boardAid.locale.toString(i), tr("Color(s)", "", i)));
	}
	colorsNumberComboBox->setToolTip(tr("Choose the number of colors"));

	if (gameRules.sameColorAllowed)
		allowSameColorAction->setToolTip(tr("Same Color Allowed"));
	else
		allowSameColorAction->setToolTip(tr("Same Color Not Allwed"));


	solvingAlgorithmsComboBox->setToolTip(tr("Choose the solving algorithm"));
	solvingAlgorithmsComboBox->setItemText(0, tr("Most Parts"));
	solvingAlgorithmsComboBox->setItemText(1, tr("Worst Case"));
	solvingAlgorithmsComboBox->setItemText(2, tr("Expected Size"));

	game->retranslateTexts();
}
//-----------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent *event)
{
	QSettings().setValue("Geometry", saveGeometry());
	QMainWindow::closeEvent(event);
}
//-----------------------------------------------------------------------------

void MainWindow::onNewGame()
{
	if (quitUnfinishedGame())
	{
		game->stop();
		updateGameRules();
		ui->actionResign->setEnabled(gameRules.gameMode == GameMode::HVM);
		ui->actionResign->setVisible(gameRules.gameMode == GameMode::HVM);
		ui->actionReveal_One_Peg->setEnabled(gameRules.gameMode == GameMode::HVM);
		ui->actionReveal_One_Peg->setVisible(gameRules.gameMode == GameMode::HVM);
		game->play();
	}
}
//-----------------------------------------------------------------------------

bool MainWindow::quitUnfinishedGame()
{
	if (!game->isRunning())
		return true;
	int new_game_accept = QMessageBox::warning(this,
								  tr("New Game"), QString("<p align='center'>%1</p>"
													"<p align='center'>%2</p>")
												.arg(tr("An unfinished game is in progress."))
												.arg(tr("Do you want to start a new game?")),
								  QMessageBox::Yes | QMessageBox::No);
	return (new_game_accept == QMessageBox::Yes);
}

//-----------------------------------------------------------------------------

void MainWindow::onPreferences()
{
	auto preferencesWidget = new Preferences(&boardAid, this);
	preferencesWidget->setWindowTitle(tr("Options"));
	preferencesWidget->exec();
	onIndicatorChanged();
	Preferences::loadTranslation(&boardAid);
	setLayoutDirection(boardAid.locale.textDirection());
	resetActionsText();

}
//-----------------------------------------------------------------------------

void MainWindow::onQtMindHomePage()
{
	QDesktopServices::openUrl(QUrl("http://omidnikta.github.io/qtmind/index.html", QUrl::TolerantMode));
}
//-----------------------------------------------------------------------------

void MainWindow::onAbout()
{
	QStringList app_version = QCoreApplication::applicationVersion().split('.');
	QString localized_app_version = "";
	for(auto sub_version_number : app_version)
	{
		localized_app_version.append(boardAid.locale.toString(sub_version_number.toInt()));
		localized_app_version.append(boardAid.locale.decimalPoint());
	}
	localized_app_version.chop(1);
	QMessageBox::about(this, tr("About QtMind"), QString(
		"<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>")
		.arg(tr("QtMind"), localized_app_version,
			tr("Code Breaking Game, A Clone Of The Mastermind Board Game"),
			tr("Copyright &copy; 2013-%1 Omid Nikta").arg(boardAid.locale.toString(2013)),
			tr("Released under the <a href=%1>GPL 3</a> license").arg("\"http://www.gnu.org/licenses/gpl.html\"")));
}
//-----------------------------------------------------------------------------

void MainWindow::onAutoPutPins()
{
	boardAid.autoPutPins = ui->actionAuto_Set_Pins->isChecked();
}
//-----------------------------------------------------------------------------
void MainWindow::onAutoCloseRows()
{
	boardAid.autoCloseRows = ui->actionAuto_Close_Rows->isChecked();
}
//-----------------------------------------------------------------------------

void MainWindow::onIndicatorChanged()
{
	boardAid.indicator.setShowIndicators(ui->actionShow_Indicators->isChecked());
	game->changeIndicators();
}
//-----------------------------------------------------------------------------

void MainWindow::onGameModeChanged(QAction *selectedAction)
{
	Q_UNUSED(selectedAction);
	if (getGameMode() != gameRules.gameMode)
		onNewGame();
}
//-----------------------------------------------------------------------------

void MainWindow::onAlgorithmChanded()
{
	gameRules.algorithm = (Algorithm) solvingAlgorithmsComboBox->currentIndex();

}
//-----------------------------------------------------------------------------

void MainWindow::onShowContextMenu(const QPoint &position)
{
	QMenu contextMenu(this);
	contextMenu.addAction(ui->actionNew);
	contextMenu.addAction(ui->actionReveal_One_Peg);
	contextMenu.addAction(ui->actionResign);
	contextMenu.addSeparator();
	contextMenu.addMenu(ui->menuGameMode);
	contextMenu.exec(mapToGlobal(position));
}
//-----------------------------------------------------------------------------

void MainWindow::updateGameRules()
{
	gameRules.gameMode = getGameMode();
	gameRules.colorNumber = colorsNumberComboBox->currentIndex() + MIN_COLOR_NUMBER;
	gameRules.pegNumber = pegsNumberComboBox->currentIndex() + MIN_SLOT_NUMBER;
	gameRules.algorithm = (Algorithm) solvingAlgorithmsComboBox->currentIndex();
	gameRules.sameColorAllowed = (allowSameColorAction->isChecked()) || (gameRules.pegNumber > gameRules.colorNumber);
	allowSameColorAction->setChecked(gameRules.sameColorAllowed);
	if (gameRules.sameColorAllowed)
		allowSameColorAction->setToolTip(tr("Same Color Allowed"));
	else
		allowSameColorAction->setToolTip(tr("Same Color Not Allwed"));

	// for safety, fallback to standard in out-range inputs
	if (gameRules.pegNumber < MIN_SLOT_NUMBER || gameRules.pegNumber > MAX_SLOT_NUMBER ||
			gameRules.colorNumber < MIN_COLOR_NUMBER || gameRules.pegNumber > MAX_COLOR_NUMBER)
	{
		gameRules.pegNumber = 4;
		gameRules.colorNumber = 6;
	}
}
//-----------------------------------------------------------------------------

GameMode MainWindow::getGameMode()
{
	if (ui->actionGameMode_MVH->isChecked())
		return GameMode::MVH;
	else	//if (ui->actionGameMode_HVM->isChecked())
		return GameMode::HVM;
}
