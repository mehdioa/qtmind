#include "mainwindow.h"

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

#include "board.h"


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	setWindowTitle(tr("CodeBreak"));
	m_mode = QSettings().value("Mode", 0).toInt();
	m_colors = QSettings().value("Colors", 8).toInt()-2;
	m_pegs = QSettings().value("Pegs", 6).toInt()-2;
	m_algorithm = QSettings().value("Algorithm", 0).toInt();
	m_allowSameColor = QSettings().value("SameColor", true).toBool();
	m_setPins = QSettings().value("SetPins", true).toBool();
	m_closeRow = QSettings().value("CloseRow", true).toBool();

	createMenuBar();

	gameBoard = new Board(this);
	setCentralWidget(gameBoard);

	resize(400,400);
	restoreGeometry(QSettings().value("Geometry").toByteArray());

	newGameSlot();
}

MainWindow::~MainWindow()
{
}

//-----------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* event)
{
	QSettings().setValue("Mode", m_mode);
	QSettings().setValue("Colors", m_colors+2);
	QSettings().setValue("Pegs", m_pegs+2);
	QSettings().setValue("Algorithm", m_algorithm);
	QSettings().setValue("SameColor", m_allowSameColor);
	QSettings().setValue("SetPins",	m_setPins);
	QSettings().setValue("CloseRow", m_closeRow);
	QSettings().setValue("Geometry", saveGeometry());
	QMainWindow::closeEvent(event);
}

//-----------------------------------------------------------------------------

void MainWindow::newGameSlot()
{
	m_colors = colorsNumberComboBox->currentIndex();
	m_algorithm = solvingAlgorithmsComboBox->currentIndex();
	m_pegs = pegsNumberComboBox->currentIndex();
	m_allowSameColor = toggleAllowSameColorAction->isChecked();
	gameBoard->reset(m_pegs+2, m_colors+2, m_mode, m_allowSameColor, m_algorithm, m_setPins, m_closeRow);
	gameBoard->play(m_mode);
}

//-----------------------------------------------------------------------------

void MainWindow::throwInTheTowelSlot()
{
}

//-----------------------------------------------------------------------------

void MainWindow::toggleAllowSameColorSlot()
{
	m_allowSameColor = toggleAllowSameColorAction->isChecked();
}

void MainWindow::setPinsCloseRowAutomatically()
{
	m_setPins = setPinsAutomaticallyAction->isChecked();
	m_closeRow = closeRowAutomaticallyAction->isChecked();
	gameBoard->setPinsRow(m_setPins, m_closeRow);
}
//-----------------------------------------------------------------------------

void MainWindow::doItForMeSlot()
{

}

void MainWindow::codeMasterModeSlot()
{
	m_mode = 0;
}

void MainWindow::codeBreakModeSlot()
{
	m_mode = 1;
}
//-----------------------------------------------------------------------------

void MainWindow::about()
{
	QMessageBox::about(this, tr("About Peg-E"), QString(
		"<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>"
		"<p align='center'>%6<br/><small>%7</small></p>")
		.arg(tr("Peg-E"), QCoreApplication::applicationVersion(),
			tr("Peg elimination game"),
			tr("Copyright &copy; 2009-%1 Graeme Gott").arg("2013"),
			tr("Released under the <a href=%1>GPL 3</a> license").arg("\"http://www.gnu.org/licenses/gpl.html\""),
			tr("Uses icons from the <a href=%1>Oxygen</a> icon theme").arg("\"http://www.oxygen-icons.org/\""),
			tr("Used under the <a href=%1>LGPL 3</a> license").arg("\"http://www.gnu.org/licenses/lgpl.html\""))
	);
}
//-----------------------------------------------------------------------------

//void MainWindow::startGame()
//{
//	QSettings settings;
//	settings.setValue("Mode", m_mode);
//	settings.setValue("Algorithm", m_algorithm);
//	gameBoard->reset(m_pegs, m_colors, m_mode, m_allowSameColor);

//}

//-----------------------------------------------------------------------------
void MainWindow::setPegsComboBox()
{
	pegsNumberComboBox = new QComboBox(this);
	for(int i = MIN_SLOT_NUMBER; i <= MAX_SLOT_NUMBER; ++i)
		pegsNumberComboBox->addItem(QString::number(i)+" Slots");
	pegsNumberComboBox->setCurrentIndex(m_pegs);
	pegsNumberComboBox->setToolTip("Choose the numbe of pegs");
}

void MainWindow::setColorsComboBox()
{
	colorsNumberComboBox = new QComboBox(this);
	for(int i = MIN_COLOR_NUMBER; i <= MAX_COLOR_NUMBER; ++i)
		colorsNumberComboBox->addItem(QString::number(i)+" Colors");
	colorsNumberComboBox->setCurrentIndex(m_colors);
	colorsNumberComboBox->setToolTip("Choose the number of colors");
}

void MainWindow::setSolvingAlgorithmsComboBox()
{
	solvingAlgorithmsComboBox = new QComboBox(this);
	solvingAlgorithmsComboBox->setToolTip("Choose the solving algorithm");
	solvingAlgorithmsComboBox->addItem("Worst Case");
	solvingAlgorithmsComboBox->addItem("Expected Size");
	solvingAlgorithmsComboBox->addItem("Most Parts");
	solvingAlgorithmsComboBox->setCurrentIndex(m_algorithm);
}


void MainWindow::createMenuBar()
{
	auto gameMenu = menuBar()->addMenu(tr("&Game"));

	QIcon new_icon(QPixmap(":icons/new-icon.png"));
//	new_icon.addPixmap(QPixmap(":/16x16/document-new.png"));
	auto newGameAction = gameMenu->addAction(new_icon, tr("&New"), this, SLOT(newGameSlot()), tr("Ctrl+N"));

//	QIcon restart_icon(QPixmap(":/icons/restart-icon.png"));
//	auto restartGameAction = gameMenu->addAction(restart_icon, tr("&Restart Game"), this, SLOT(restartGameSlot()), tr("F5"));

	QIcon throwtowel_icon(QPixmap(":/icons/face-raspberry.png"));
	auto throwInTheTowelAction = gameMenu->addAction(throwtowel_icon, tr("&Throw In The Towel"), this, SLOT(throwInTheTowelSlot()));

	gameMenu->addSeparator();

	QIcon close_icon(QPixmap(":/icons/close-icon.png"));
	gameMenu->addAction(close_icon, tr("Quit"), this, SLOT(close()), tr("Ctrl+Q"));

	auto rowMenu = menuBar()->addMenu(tr("&Row"));

	QIcon do_icon;
	do_icon.addPixmap(QPixmap(":/icons/monkey-icon.png"));
	auto doItForMeAction = rowMenu->addAction(do_icon, tr("&Do It For Me"), this, SLOT(doItForMeSlot()), tr("F6"));

	auto settingsMenu = menuBar()->addMenu(tr("&Settings"));

	auto codeMasterModeAction = settingsMenu->addAction(tr("&Code Master"), this, SLOT(codeMasterModeSlot()), tr("Ctrl+M"));
	codeMasterModeAction->setCheckable(true);
	codeMasterModeAction->setChecked(m_mode == 0);
	auto codeBreakModeAction = settingsMenu->addAction(tr("&Code Break"), this, SLOT(codeBreakModeSlot()), tr("Ctrl+B"));
	codeBreakModeAction->setCheckable(true);
	codeBreakModeAction->setChecked(m_mode == 1);

	gameModeActions = new QActionGroup(this);
	gameModeActions->addAction(codeMasterModeAction);
	gameModeActions->addAction(codeBreakModeAction);
	gameModeActions->setExclusive(true);

	QIcon double_icon;
	double_icon.addPixmap(QPixmap(":/icons/same_color_1.png"), QIcon::Normal, QIcon::On);
	double_icon.addPixmap(QPixmap(":/icons/same_color_0.png"), QIcon::Normal, QIcon::Off);
	toggleAllowSameColorAction = settingsMenu->addAction(double_icon, tr("&Toggle Allow Pegs Of The Same Color"), this, SLOT(toggleAllowSameColorSlot()), tr("F5"));
	toggleAllowSameColorAction->setCheckable(true);
	toggleAllowSameColorAction->setChecked(m_allowSameColor);

	setPinsAutomaticallyAction = settingsMenu->addAction(tr("&Set Pins Automatically"), this, SLOT(setPinsCloseRowAutomatically()), tr("Ctrl+P"));
	setPinsAutomaticallyAction->setCheckable(true);
	setPinsAutomaticallyAction->setChecked(m_setPins == 1);

	closeRowAutomaticallyAction = settingsMenu->addAction(tr("&Clost Row Automatically"), this, SLOT(setPinsCloseRowAutomatically()), tr("Ctrl+R"));
	closeRowAutomaticallyAction->setCheckable(true);
	closeRowAutomaticallyAction->setChecked(m_closeRow == 1);

	auto helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(tr("&About"), this, SLOT(about()));
	helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));

	setPegsComboBox();
	setColorsComboBox();
	setSolvingAlgorithmsComboBox();


	auto mainToolbar = new QToolBar(this);
	mainToolbar->setIconSize(QSize(22, 22));
	mainToolbar->setFloatable(false);
	mainToolbar->setMovable(false);
	mainToolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
	mainToolbar->addAction(newGameAction);
//	mainToolbar->addAction(restartGameAction);
	mainToolbar->addAction(throwInTheTowelAction);
	mainToolbar->addAction(doItForMeAction);
	mainToolbar->addAction(toggleAllowSameColorAction);
	mainToolbar->addSeparator();
	mainToolbar->addWidget(pegsNumberComboBox);
	mainToolbar->addWidget(colorsNumberComboBox);
	mainToolbar->addWidget(solvingAlgorithmsComboBox);
	addToolBar(mainToolbar);
	setContextMenuPolicy(Qt::NoContextMenu);
}


//void MainWindow::createToolBar()
//{
//	mainToolbar = new QToolBar(this);
//	mainToolbar->setIconSize(QSize(22, 22));
//	mainToolbar->setFloatable(false);
//	mainToolbar->setMovable(false);
//	mainToolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
//	mainToolbar->addAction(newGameAction);
//	mainToolbar->addAction(restartGameAction);
//	mainToolbar->addAction(throwInTheTowelAction);
//	mainToolbar->addAction(doItForMeAction);
//	mainToolbar->addAction(toggleAllowSameColorAction);
//	mainToolbar->addSeparator();
//	mainToolbar->addWidget(pegsNumberComboBox);
//	mainToolbar->addWidget(colorsNumberComboBox);
//	mainToolbar->addWidget(solvingAlgorithmsComboBox);
//	addToolBar(mainToolbar);
//	setContextMenuPolicy(Qt::NoContextMenu);
//}
