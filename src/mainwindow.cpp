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
#include <QFontComboBox>
#include <QFile>
#include <QLibraryInfo>
#include <QDir>
#include <QTranslator>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	boardAid.locale.setNumberOptions(QLocale::OmitGroupSeparator);
	loadTranslation();
	QApplication::setLayoutDirection(boardAid.locale.textDirection());

	game = new Game(&gameRules, &boardAid, this);
	setCentralWidget(game);

	auto gameModeActions = new QActionGroup(this);
	gameModeActions->addAction(ui->actionGameMode_MVH);
	gameModeActions->addAction(ui->actionGameMode_HVM);
	gameModeActions->setExclusive(true);
	ui->actionGameMode_MVH->setData((int) GameMode::MVH);
	ui->actionGameMode_HVM->setData((int) GameMode::HVM);
	ui->actionGameMode_MVH->setChecked(gameRules.gameMode == GameMode::MVH);
	ui->actionGameMode_HVM->setChecked(gameRules.gameMode == GameMode::HVM);

	auto volumeModeActions = new QActionGroup(this);
	volumeModeActions->addAction(ui->actionMute);
	volumeModeActions->addAction(ui->actionLow);
	volumeModeActions->addAction(ui->actionMedium);
	volumeModeActions->addAction(ui->actionHigh);
	volumeModeActions->setExclusive(true);
	ui->actionMute->setData(static_cast<int>(BoardSounds::Mute));
	ui->actionLow->setData(static_cast<int>(BoardSounds::Low));
	ui->actionMedium->setData(static_cast<int>(BoardSounds::Medium));
	ui->actionHigh->setData(static_cast<int>(BoardSounds::High));
	ui->actionMute->setChecked(boardAid.boardSounds.getVolume() == BoardSounds::Mute);
	ui->actionLow->setChecked(boardAid.boardSounds.getVolume() == BoardSounds::Low);
	ui->actionMedium->setChecked(boardAid.boardSounds.getVolume() == BoardSounds::Medium);
	ui->actionHigh->setChecked(boardAid.boardSounds.getVolume() == BoardSounds::High);

	QIcon double_icon;
	double_icon.addPixmap(QPixmap("://icons/resources/icons/same_color_1.png"), QIcon::Normal, QIcon::On);
	double_icon.addPixmap(QPixmap("://icons/resources/icons/same_color_0.png"), QIcon::Normal, QIcon::Off);
	ui->actionAllow_Same_Colors->setIcon(double_icon);
	ui->actionAllow_Same_Colors->setChecked(gameRules.sameColorAllowed);

	ui->menuIndicators->actions().at(0)->setChecked(boardAid.indicator.showIndicators);
	ui->menuIndicators->actions().at(1)->setChecked(boardAid.indicator.showColors);
	auto indicator_types = new QActionGroup(this);
	for(int i = 0; i < 2; i++) {
		ui->menuIndicators->actions().at(i+3)->setText((i == 0) ? tr("Characters"): tr("Digits"));
		ui->menuIndicators->actions().at(i+3)->setData(i);
		ui->menuIndicators->actions().at(i+3)->setChecked(boardAid.indicator.indicatorType == ((i == 0) ?  IndicatorType::Character : IndicatorType::Digit));
		indicator_types->addAction(ui->menuIndicators->actions().at(i+3));
	}
	indicator_types->setExclusive(true);

	ui->actionAuto_Set_Pins->setChecked(boardAid.autoPutPins);
	ui->actionAuto_Close_Rows->setChecked(boardAid.autoCloseRows);

	QStringList translations = findTranslations();
	QString app_name_ = QApplication::applicationName().toLower()+"_";
	translations = translations.filter(app_name_);
	auto language_actions = new QActionGroup(this);
	foreach(QString translation, translations) {
		translation.remove(app_name_);
		auto language_act = new QAction(languageName(translation), this);
		language_act->setData(translation);
		language_act->setCheckable(true);
		language_act->setChecked(boardAid.locale.name().left(2) == translation);
		language_actions->addAction(language_act);
		ui->menuLanguage->addAction(language_act);
	}
	language_actions->setExclusive(true);


	pegsNumberComboBox = new QComboBox(this);
	auto slotActions = new QActionGroup(this);
	for(int i = MIN_SLOT_NUMBER; i <= MAX_SLOT_NUMBER; ++i) {
		pegsNumberComboBox->addItem(QString("%1 %2").arg(boardAid.locale.toString(i), tr("Slot(s)", "", i)));
		auto slot_act = new QAction(QString("%1 %2").arg(boardAid.locale.toString(i), tr("Slot(s)", "", i)), this);
		slot_act->setCheckable(true);
		slot_act->setData(i);
		slot_act->setChecked(gameRules.pegNumber == i);
		slotActions->addAction(slot_act);
		ui->menuSlots->addAction(slot_act);
	}
	slotActions->setExclusive(true);
	pegsNumberComboBox->setCurrentIndex(gameRules.pegNumber-MIN_SLOT_NUMBER);
	pegsNumberComboBox->setLocale(boardAid.locale);

	colorsNumberComboBox = new QComboBox(this);
	auto colorActions = new QActionGroup(this);
	for(int i = MIN_COLOR_NUMBER; i <= MAX_COLOR_NUMBER; ++i) {
		colorsNumberComboBox->addItem(QString("%1 %2").arg(boardAid.locale.toString(i), tr("Color(s)", "", i)));
		auto color_act = new QAction(QString("%1 %2").arg(boardAid.locale.toString(i), tr("Color(s)", "", i)), this);
		color_act->setCheckable(true);
		color_act->setData(i);
		color_act->setChecked(gameRules.colorNumber == i);
		colorActions->addAction(color_act);
		ui->menuColors->addAction(color_act);
	}
	colorActions->setExclusive(true);
	colorsNumberComboBox->setCurrentIndex(gameRules.colorNumber-MIN_COLOR_NUMBER);

	solvingAlgorithmsComboBox = new QComboBox(this);
	solvingAlgorithmsComboBox->setToolTip(tr("Choose the solving algorithm"));
	solvingAlgorithmsComboBox->addItem(tr("Most Parts"), 0);
	solvingAlgorithmsComboBox->addItem(tr("Worst Case"), 1);
	solvingAlgorithmsComboBox->addItem(tr("Expected Size"), 2);
	solvingAlgorithmsComboBox->setCurrentIndex((int) gameRules.algorithm);

	auto algorithmActions = new QActionGroup(this);
	for(int i = 0; i < 3; i++) {
		auto alg_act = new QAction((i == 0) ? tr("Most Parts") : ((i == 1) ? tr("Worst Case"): tr("Expected Size")), this);
		alg_act->setCheckable(true);
		alg_act->setData(i);
		alg_act->setChecked(gameRules.algorithm == static_cast<Algorithm>(i));
		algorithmActions->addAction(alg_act);
		ui->menuAlgorithm->addAction(alg_act);
	}
	algorithmActions->setExclusive(true);

	ui->toolBar->addAction(ui->actionNew);
	ui->toolBar->addAction(ui->actionAllow_Same_Colors);
	ui->toolBar->addSeparator();
	ui->toolBar->addWidget(pegsNumberComboBox);
	ui->toolBar->addWidget(colorsNumberComboBox);
	ui->toolBar->addWidget(solvingAlgorithmsComboBox);

	connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(onNewGame()));
	connect(ui->actionReveal_One_Peg, SIGNAL(triggered()), game, SLOT(onRevealOnePeg()));
	connect(ui->actionResign, SIGNAL(triggered()), game, SLOT(onResigned()));
	connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(gameModeActions, SIGNAL(triggered(QAction*)), this, SLOT(onGameModeChanged(QAction*)));
	connect(volumeModeActions, SIGNAL(triggered(QAction*)), this, SLOT(onVolumeChanged(QAction*)));
	connect(colorActions, SIGNAL(triggered(QAction*)), this, SLOT(onColorActionChanged(QAction*)));
	connect(colorsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onColorComboChanged(int)));
	connect(slotActions, SIGNAL(triggered(QAction*)), this, SLOT(onSlotActionChanged(QAction*)));
	connect(pegsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSlotComboChanged(int)));
	connect(ui->menuAlgorithm, SIGNAL(triggered(QAction*)), this, SLOT(onAlgorithmActionChanged(QAction*)));
	connect(solvingAlgorithmsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAlgorithmComboChanged(int)));
	connect(ui->menuIndicators->actions().at(0), SIGNAL(triggered()), this, SLOT(onIndicatorChanged()));
	connect(ui->menuIndicators->actions().at(1), SIGNAL(triggered()), this, SLOT(onIndicatorChanged()));
	connect(ui->actionAllow_Same_Colors, SIGNAL(triggered()), this, SLOT(onNewGame()));
	connect(ui->actionAuto_Set_Pins, SIGNAL(triggered()), this, SLOT(onAutoPutPins()));
	connect(ui->actionAuto_Close_Rows, SIGNAL(triggered()), this, SLOT(onAutoCloseRows()));
	connect(ui->actionFont, SIGNAL(triggered()), this, SLOT(onFont()));
	connect(ui->actionQtMind_Home_Page, SIGNAL(triggered()), this, SLOT(onQtMindHomePage()));
	connect(ui->actionAbout_QtMind, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(colorsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onNewGame()));
	connect(pegsNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onNewGame()));
	connect(solvingAlgorithmsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAlgorithmChanded()));
	connect(ui->menuLanguage, SIGNAL(triggered(QAction*)), this, SLOT(onLanguageChanged(QAction*)));

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onShowContextMenu(QPoint)));
	connect(indicator_types, SIGNAL(triggered(QAction*)), this, SLOT(onIndicatorTypeChanged(QAction*)));
	onNewGame();
	game->changeIndicators();

	retranslate();
	restoreGeometry(QSettings().value("Geometry").toByteArray());

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
	ui->menuVolume->setEnabled(false);
	ui->menuVolume->setVisible(false);
#endif

#ifdef Q_OS_ANDROID
	ui->actionFont->setVisible(false);
#endif
}
//-----------------------------------------------------------------------------

MainWindow::~MainWindow()
{
	delete ui;
}
//-----------------------------------------------------------------------------

void MainWindow::retranslate()
{
	setWindowTitle(tr("QtMind"));
	ui->menuGame->setTitle(tr("&Game"));
	ui->actionNew->setText(tr("&New"));
	ui->menuAlgorithm->setTitle(tr("&Algorithm"));
	ui->menuAlgorithm->actions().at(0)->setText(tr("&Most Parts"));
	ui->menuAlgorithm->actions().at(1)->setText(tr("&Worst Case"));
	ui->menuAlgorithm->actions().at(2)->setText(tr("&Expected Size"));
	ui->menuColors->setTitle(tr("&Colors"));
	ui->menuSlots->setTitle(tr("&Slots"));
	ui->actionReveal_One_Peg->setText(tr("Reveal One &Peg"));
	ui->actionResign->setText(tr("&Resign"));
	ui->actionQuit->setText(tr("&Quit"));

	ui->menuTools->setTitle(tr("&Tools"));
	ui->menuGameMode->setTitle(tr("Game &Mode"));
	ui->actionGameMode_MVH->setText(tr("&Machine vs Human"));
	ui->actionGameMode_HVM->setText(tr("&Human vs Machine"));
	ui->menuIndicators->setTitle(tr("&Indicators"));
	ui->menuIndicators->actions().at(0)->setText(tr("&Show"));
	ui->menuIndicators->actions().at(1)->setText(tr("Show &Colors"));
	ui->menuIndicators->actions().at(3)->setText(tr("&Characters"));
	ui->menuIndicators->actions().at(4)->setText(tr("&Digits"));
	ui->actionAuto_Close_Rows->setText(tr("Auto Close &Rows"));
	ui->actionAuto_Set_Pins->setText(tr("Auto Put &Pins"));
	ui->menuLanguage->setTitle(tr("&Language"));
	ui->menuVolume->setTitle(tr("&Volume"));
	ui->menuVolume->actions().at(0)->setText(tr("&Mute"));
	ui->menuVolume->actions().at(1)->setText(tr("&Low"));
	ui->menuVolume->actions().at(2)->setText(tr("&Medium"));
	ui->menuVolume->actions().at(3)->setText(tr("&High"));
	ui->actionFont->setText(tr("&Font"));

	ui->menuHelp->setTitle(tr("&Help"));
	ui->actionQtMind_Home_Page->setText(tr("QtMind &Home Page"));
	ui->actionAbout_QtMind->setText(tr("About Qt&Mind"));
	ui->actionAbout_Qt->setText(tr("About &Qt"));


	for(int i = MIN_SLOT_NUMBER; i <= MAX_SLOT_NUMBER; ++i) {
		pegsNumberComboBox->setItemText(i - MIN_SLOT_NUMBER, QString("%1 %2").arg(boardAid.locale.toString(i), tr("Slot(s)", "", i)));
		ui->menuSlots->actions().at(i - MIN_SLOT_NUMBER)->setText(QString("%1 %2").arg(boardAid.locale.toString(i), tr("Slot(s)", "", i)));
	}
	pegsNumberComboBox->setToolTip(tr("Choose the numbe of slots"));

	for(int i = MIN_COLOR_NUMBER; i <= MAX_COLOR_NUMBER; ++i) {
		colorsNumberComboBox->setItemText(i - MIN_COLOR_NUMBER, QString("%1 %2").arg(boardAid.locale.toString(i), tr("Color(s)", "", i)));
		ui->menuColors->actions().at(i - MIN_COLOR_NUMBER)->setText(QString("%1 %2").arg(boardAid.locale.toString(i), tr("Color(s)", "", i)));
	}
	colorsNumberComboBox->setToolTip(tr("Choose the number of colors"));

	ui->actionAllow_Same_Colors->setText(tr("&Allow Same Color"));
	if (gameRules.sameColorAllowed)
		ui->actionAllow_Same_Colors->setToolTip(tr("Same Color Allowed"));
	else
		ui->actionAllow_Same_Colors->setToolTip(tr("Same Color Not Allwed"));

	solvingAlgorithmsComboBox->setToolTip(tr("Choose the solving algorithm"));
	solvingAlgorithmsComboBox->setItemText(0, tr("Most Parts"));
	solvingAlgorithmsComboBox->setItemText(1, tr("Worst Case"));
	solvingAlgorithmsComboBox->setItemText(2, tr("Expected Size"));

	game->retranslateTexts();
}

void MainWindow::setPegsNumber(const int &pegs_n)
{
	pegsNumberComboBox->setCurrentIndex(pegs_n);
}
//-----------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (game->isRunning() && QMessageBox::No == QMessageBox::warning(this,
																	   tr("Quit"), QString(boardAid.isAndroid ? "%1\n%2" : "<p align='center'>%1</p>"
																						 "<p align='center'>%2</p>")
																					 .arg(tr("An unfinished game is in progress."))
																					 .arg(tr("Do you want to quit?")),
																	   QMessageBox::Yes | QMessageBox::No)) {
		event->ignore();
	} else {
		game->stop();
		QSettings().setValue("Geometry", saveGeometry());
		QMainWindow::closeEvent(event);
	}
}
//-----------------------------------------------------------------------------

void MainWindow::onNewGame()
{
	if (quitUnfinishedGame()) {
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
								  tr("New Game"), QString(boardAid.isAndroid ? "%1\n%2" : "<p align='center'>%1</p>"
													"<p align='center'>%2</p>")
												.arg(tr("An unfinished game is in progress."))
												.arg(tr("Do you want to start a new game?")),
								  QMessageBox::Yes | QMessageBox::No);
	return (new_game_accept == QMessageBox::Yes);
}

//-----------------------------------------------------------------------------

void MainWindow::onFont()
{
	auto preferencesWidget = new Preferences(&boardAid, this);
	preferencesWidget->setModal(true);
	preferencesWidget->setWindowTitle(tr("Font"));
	preferencesWidget->exec();
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
	foreach(QString sub_version_number, app_version) {
		localized_app_version.append(boardAid.locale.toString(sub_version_number.toInt()));
		localized_app_version.append(boardAid.locale.decimalPoint());
	}
	localized_app_version.chop(1);
	QMessageBox::about(this, tr("About QtMind"), QString(boardAid.isAndroid ? "%1 %2\n%3\n\n%4\n%5" :
		"<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>")
		.arg(tr("QtMind"), localized_app_version,
			tr("Code Breaking Game, A Clone Of The Mastermind Board Game"),
			tr("Copyright &copy; 2013-%1 Omid Nikta").arg(boardAid.locale.toString(2014)),
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
	boardAid.indicator.showIndicators = ui->menuIndicators->actions().at(0)->isChecked();
	boardAid.indicator.showColors = ui->menuIndicators->actions().at(1)->isChecked();
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
	gameRules.algorithm = static_cast<Algorithm>(solvingAlgorithmsComboBox->currentIndex());

}
//-----------------------------------------------------------------------------

void MainWindow::onVolumeChanged(QAction *volume_action)
{
	boardAid.boardSounds.setVolume(volume_action->data().toInt());
}
//-----------------------------------------------------------------------------

void MainWindow::onColorActionChanged(QAction *color_action)
{
	if (colorsNumberComboBox->currentIndex() != color_action->data().toInt() - 2) {
		colorsNumberComboBox->setCurrentIndex(color_action->data().toInt() - MIN_COLOR_NUMBER);
	}
}
//-----------------------------------------------------------------------------

void MainWindow::onColorComboChanged(const int &combo_index)
{
	if (ui->menuColors->menuAction()->data().toInt() - 2 != combo_index) {
		ui->menuColors->actions().at(combo_index)->setChecked(true);
	}
}
//-----------------------------------------------------------------------------

void MainWindow::onSlotActionChanged(QAction *slot_action)
{
	if (pegsNumberComboBox->currentIndex() != slot_action->data().toInt() - 2) {
		pegsNumberComboBox->setCurrentIndex(slot_action->data().toInt() - MIN_SLOT_NUMBER);
	}
}
//-----------------------------------------------------------------------------

void MainWindow::onSlotComboChanged(const int &combo_index)
{
	if (ui->menuSlots->menuAction()->data().toInt() - 2 != combo_index) {
		ui->menuSlots->actions().at(combo_index)->setChecked(true);
	}
}
//-----------------------------------------------------------------------------

void MainWindow::onAlgorithmActionChanged(QAction *algorithm_action)
{
	if (solvingAlgorithmsComboBox->currentIndex() != algorithm_action->data().toInt()) {
		solvingAlgorithmsComboBox->setCurrentIndex(algorithm_action->data().toInt());
	}
}
//-----------------------------------------------------------------------------

void MainWindow::onAlgorithmComboChanged(const int &combo_index)
{
	if (ui->menuAlgorithm->menuAction()->data().toInt() != combo_index) {
		ui->menuAlgorithm->actions().at(combo_index)->setChecked(true);
	}
}
//-----------------------------------------------------------------------------

void MainWindow::onIndicatorTypeChanged(QAction *indic_act)
{
	boardAid.indicator.indicatorType = static_cast<IndicatorType>(indic_act->data().toInt());
	game->changeIndicators();
}
//-----------------------------------------------------------------------------

void MainWindow::onLanguageChanged(QAction *language_act)
{
	QLocale newLocale(language_act->data().toString());
	boardAid.locale = newLocale;
	boardAid.locale.setNumberOptions(QLocale::OmitGroupSeparator);
	loadTranslation();
	setLayoutDirection(boardAid.locale.textDirection());
	retranslate();
}
//-----------------------------------------------------------------------------

void MainWindow::onShowContextMenu(const QPoint &position)
{
	QMenu contextMenu(this);
	contextMenu.addAction(ui->actionNew);
	contextMenu.addMenu(ui->menuSlots);
	contextMenu.addMenu(ui->menuColors);
	contextMenu.addMenu(ui->menuAlgorithm);
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
	gameRules.algorithm = static_cast<Algorithm>(solvingAlgorithmsComboBox->currentIndex());
	gameRules.sameColorAllowed = (ui->actionAllow_Same_Colors->isChecked()) || (gameRules.pegNumber > gameRules.colorNumber);
	ui->actionAllow_Same_Colors->setChecked(gameRules.sameColorAllowed);
	if (gameRules.sameColorAllowed)
		ui->actionAllow_Same_Colors->setToolTip(tr("Same Color Allowed"));
	else
		ui->actionAllow_Same_Colors->setToolTip(tr("Same Color Not Allwed"));

	// for safety, fallback to standard in out-range inputs
	if (gameRules.pegNumber < MIN_SLOT_NUMBER || gameRules.pegNumber > MAX_SLOT_NUMBER ||
			gameRules.colorNumber < MIN_COLOR_NUMBER || gameRules.pegNumber > MAX_COLOR_NUMBER) {
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
//-----------------------------------------------------------------------------

QStringList MainWindow::findTranslations()
{
	QStringList result = QDir(AppPath, "*.qm").entryList(QDir::Files);
	result.replaceInStrings(".qm", "");
	return result;
}
//-----------------------------------------------------------------------------

void MainWindow::loadTranslation()
{
	QString appdir = QCoreApplication::applicationDirPath();
	QString app_name_ = QApplication::applicationName().toLower()+"_";
	// Find translator path
	QStringList paths;
	paths.append("assets:/translations");// Android
	paths.append(appdir + "/translations/");// Windows
	paths.append(appdir + "/../share/" + QApplication::applicationName().toLower() + "/translations/");// *nix
	paths.append(appdir + "/../Resources/translations");// Mac
	foreach(QString path, paths)
	{
		if (QFile::exists(path)) {
			AppPath = path;
			break;
		}
	}

	// Find current locale
	QString current = boardAid.locale.name();
	QStringList translations = findTranslations();
	if (!translations.contains(app_name_ + current)) {
		current = current.left(2);
		if (!translations.contains(app_name_
								   + current)) {
			current.clear();
		}
	}
	if (!current.isEmpty()) {
		QLocale::setDefault(current);
	} else {
		current = "en";
	}

	// Load translators
	static QTranslator qt_translator;
	if (translations.contains("qt_" + current) || translations.contains("qt_" + current.left(2))) {
		qt_translator.load("qt_" + current, AppPath);
	} else {
		qt_translator.load("qt_" + current, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	}
	QCoreApplication::installTranslator(&qt_translator);

	static QTranslator translator;
	translator.load(app_name_ + current, AppPath);
	QCoreApplication::installTranslator(&translator);
}
//-----------------------------------------------------------------------------

QString MainWindow::languageName(const QString &language)
{
	QString lang_code = language.left(5);
	QLocale locale(lang_code);
	QString name;
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
	if (lang_code.length() > 2) {
		if (locale.name() == lang_code) {
			name = locale.nativeLanguageName() + " (" + locale.nativeCountryName() + ")";
		} else {
			name = locale.nativeLanguageName() + " (" + language + ")";
		}
	} else {
		name = locale.nativeLanguageName();
	}
	if (locale.textDirection() == Qt::RightToLeft) {
		name.prepend(QChar(0x202b));
	}
#else
	if (lang_code.length() > 2) {
		if (locale.name() == lang_code) {
			name = QLocale::languageToString(locale.language()) + " (" + QLocale::countryToString(locale.country()) + ")";
		} else {
			name = QLocale::languageToString(locale.language()) + " (" + language + ")";
		}
	} else {
		name = QLocale::languageToString(locale.language());
	}
#endif
	return name;
}
