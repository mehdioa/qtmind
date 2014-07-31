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
#include "rules.h"
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

	board.locale.setNumberOptions(QLocale::OmitGroupSeparator);
	loadTranslation();
	QApplication::setLayoutDirection(board.locale.textDirection());

	game = new Game(&rules, &board, this);
	setCentralWidget(game);

	auto modeActions = new QActionGroup(this);
	modeActions->addAction(ui->actionMode_MVH);
	modeActions->addAction(ui->actionMode_HVM);
	modeActions->setExclusive(true);
	ui->actionMode_MVH->setData((int) Rules::Mode::MVH);
	ui->actionMode_HVM->setData((int) Rules::Mode::HVM);
	ui->actionMode_MVH->setChecked(rules.mode == Rules::Mode::MVH);
	ui->actionMode_HVM->setChecked(rules.mode == Rules::Mode::HVM);

	auto volumeModeActions = new QActionGroup(this);
	volumeModeActions->addAction(ui->actionMute);
	volumeModeActions->addAction(ui->actionLow);
	volumeModeActions->addAction(ui->actionMedium);
	volumeModeActions->addAction(ui->actionHigh);
	volumeModeActions->setExclusive(true);
	ui->actionMute->setData(static_cast<int>(Sounds::Mute));
	ui->actionLow->setData(static_cast<int>(Sounds::Low));
	ui->actionMedium->setData(static_cast<int>(Sounds::Medium));
	ui->actionHigh->setData(static_cast<int>(Sounds::High));
	ui->actionMute->setChecked(board.sounds.getVolume() == Sounds::Mute);
	ui->actionLow->setChecked(board.sounds.getVolume() == Sounds::Low);
	ui->actionMedium->setChecked(board.sounds.getVolume() == Sounds::Medium);
	ui->actionHigh->setChecked(board.sounds.getVolume() == Sounds::High);

	QIcon double_icon;
	double_icon.addPixmap(QPixmap("://icons/resources/icons/same_color_1.png"), QIcon::Normal, QIcon::On);
	double_icon.addPixmap(QPixmap("://icons/resources/icons/same_color_0.png"), QIcon::Normal, QIcon::Off);
	ui->actionAllow_Same_Colors->setIcon(double_icon);
	ui->actionAllow_Same_Colors->setChecked(rules.same_colors);

	ui->menuIndicators->actions().at(0)->setChecked(board.indicator.showIndicators);
	ui->menuIndicators->actions().at(1)->setChecked(board.indicator.showColors);
	auto indicator_types = new QActionGroup(this);
	for(int i = 0; i < 2; i++) {
		ui->menuIndicators->actions().at(i+3)->setText((i == 0) ? tr("Characters"): tr("Digits"));
		ui->menuIndicators->actions().at(i+3)->setData(i);
		ui->menuIndicators->actions().at(i+3)->setChecked(board.indicator.type == ((i == 0) ?  Indicator::Type::Character : Indicator::Type::Digit));
		indicator_types->addAction(ui->menuIndicators->actions().at(i+3));
	}
	indicator_types->setExclusive(true);

	ui->actionAuto_Set_Pins->setChecked(board.autoPutPins);
	ui->actionAuto_Close_Rows->setChecked(board.autoCloseRows);

	QStringList translations = findTranslations();
	QString app_name_ = QApplication::applicationName().toLower()+"_";
	translations = translations.filter(app_name_);
	auto language_actions = new QActionGroup(this);
	foreach(QString translation, translations) {
		translation.remove(app_name_);
		auto language_act = new QAction(languageName(translation), this);
		language_act->setData(translation);
		language_act->setCheckable(true);
		language_act->setChecked(board.locale.name().left(2) == translation);
		language_actions->addAction(language_act);
		ui->menuLanguage->addAction(language_act);
	}
	language_actions->setExclusive(true);


	pegsComboBox = new QComboBox(this);
	auto slotActions = new QActionGroup(this);
	for(int i = Rules::MIN_SLOT_NUMBER; i <= Rules::MAX_SLOT_NUMBER; ++i) {
		pegsComboBox->addItem(QString("%1 %2").arg(board.locale.toString(i), tr("Slot(s)", "", i)));
		auto slot_act = new QAction(QString("%1 %2").arg(board.locale.toString(i), tr("Slot(s)", "", i)), this);
		slot_act->setCheckable(true);
		slot_act->setData(i);
		slot_act->setChecked(rules.pegs == i);
		slotActions->addAction(slot_act);
		ui->menuSlots->addAction(slot_act);
	}
	slotActions->setExclusive(true);
	pegsComboBox->setCurrentIndex(rules.pegs-Rules::MIN_SLOT_NUMBER);
	pegsComboBox->setLocale(board.locale);

	colorsComboBox = new QComboBox(this);
	auto colorActions = new QActionGroup(this);
	for(int i = Rules::MIN_COLOR_NUMBER; i <= Rules::MAX_COLOR_NUMBER; ++i) {
		colorsComboBox->addItem(QString("%1 %2").arg(board.locale.toString(i), tr("Color(s)", "", i)));
		auto color_act = new QAction(QString("%1 %2").arg(board.locale.toString(i), tr("Color(s)", "", i)), this);
		color_act->setCheckable(true);
		color_act->setData(i);
		color_act->setChecked(rules.colors == i);
		colorActions->addAction(color_act);
		ui->menuColors->addAction(color_act);
	}
	colorActions->setExclusive(true);
	colorsComboBox->setCurrentIndex(rules.colors-Rules::MIN_COLOR_NUMBER);

	algorithmsComboBox = new QComboBox(this);
	algorithmsComboBox->setToolTip(tr("Choose the solving algorithm"));
	algorithmsComboBox->addItem(tr("Most Parts"), 0);
	algorithmsComboBox->addItem(tr("Worst Case"), 1);
	algorithmsComboBox->addItem(tr("Expected Size"), 2);
	algorithmsComboBox->setCurrentIndex((int) rules.algorithm);

	auto algorithmActions = new QActionGroup(this);
	for(int i = 0; i < 3; i++) {
		auto alg_act = new QAction((i == 0) ? tr("Most Parts") : ((i == 1) ? tr("Worst Case"): tr("Expected Size")), this);
		alg_act->setCheckable(true);
		alg_act->setData(i);
		alg_act->setChecked(rules.algorithm == static_cast<Rules::Algorithm>(i));
		algorithmActions->addAction(alg_act);
		ui->menuAlgorithm->addAction(alg_act);
	}
	algorithmActions->setExclusive(true);

	ui->toolBar->addAction(ui->actionNew);
	ui->toolBar->addAction(ui->actionAllow_Same_Colors);
	ui->toolBar->addSeparator();
	ui->toolBar->addWidget(pegsComboBox);
	ui->toolBar->addWidget(colorsComboBox);
	ui->toolBar->addWidget(algorithmsComboBox);

	connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(onNewGame()));
	connect(ui->actionReveal_One_Peg, SIGNAL(triggered()), game, SLOT(onRevealOnePeg()));
	connect(ui->actionResign, SIGNAL(triggered()), game, SLOT(onResigned()));
	connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	connect(modeActions, SIGNAL(triggered(QAction*)), this, SLOT(onModeChanged(QAction*)));
	connect(volumeModeActions, SIGNAL(triggered(QAction*)), this, SLOT(onVolumeChanged(QAction*)));
	connect(colorActions, SIGNAL(triggered(QAction*)), this, SLOT(onColorActionChanged(QAction*)));
	connect(colorsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onColorComboChanged(int)));
	connect(slotActions, SIGNAL(triggered(QAction*)), this, SLOT(onSlotActionChanged(QAction*)));
	connect(pegsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSlotComboChanged(int)));
	connect(ui->menuAlgorithm, SIGNAL(triggered(QAction*)), this, SLOT(onAlgorithmActionChanged(QAction*)));
	connect(algorithmsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAlgorithmComboChanged(int)));
	connect(ui->menuIndicators->actions().at(0), SIGNAL(triggered()), this, SLOT(onIndicatorChanged()));
	connect(ui->menuIndicators->actions().at(1), SIGNAL(triggered()), this, SLOT(onIndicatorChanged()));
	connect(ui->actionAllow_Same_Colors, SIGNAL(triggered()), this, SLOT(onNewGame()));
	connect(ui->actionAuto_Set_Pins, SIGNAL(triggered()), this, SLOT(onAutoPutPins()));
	connect(ui->actionAuto_Close_Rows, SIGNAL(triggered()), this, SLOT(onAutoCloseRows()));
	connect(ui->actionFont, SIGNAL(triggered()), this, SLOT(onFont()));
	connect(ui->actionQtMind_Home_Page, SIGNAL(triggered()), this, SLOT(onQtMindHomePage()));
	connect(ui->actionAbout_QtMind, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	connect(colorsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onNewGame()));
	connect(pegsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onNewGame()));
	connect(algorithmsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAlgorithmChanded()));
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

MainWindow::~MainWindow()
{
	delete ui;
}

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
	ui->menuMode->setTitle(tr("Game &Mode"));
	ui->actionMode_MVH->setText(tr("&Machine vs Human"));
	ui->actionMode_HVM->setText(tr("&Human vs Machine"));
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


	for(int i = Rules::MIN_SLOT_NUMBER; i <= Rules::MAX_SLOT_NUMBER; ++i) {
		pegsComboBox->setItemText(i - Rules::MIN_SLOT_NUMBER, QString("%1 %2").arg(board.locale.toString(i), tr("Slot(s)", "", i)));
		ui->menuSlots->actions().at(i - Rules::MIN_SLOT_NUMBER)->setText(QString("%1 %2").arg(board.locale.toString(i), tr("Slot(s)", "", i)));
	}
	pegsComboBox->setToolTip(tr("Choose the numbe of slots"));

	for(int i = Rules::MIN_COLOR_NUMBER; i <= Rules::MAX_COLOR_NUMBER; ++i) {
		colorsComboBox->setItemText(i - Rules::MIN_COLOR_NUMBER, QString("%1 %2").arg(board.locale.toString(i), tr("Color(s)", "", i)));
		ui->menuColors->actions().at(i - Rules::MIN_COLOR_NUMBER)->setText(QString("%1 %2").arg(board.locale.toString(i), tr("Color(s)", "", i)));
	}
	colorsComboBox->setToolTip(tr("Choose the number of colors"));

	ui->actionAllow_Same_Colors->setText(tr("&Allow Same Color"));
	if (rules.same_colors)
		ui->actionAllow_Same_Colors->setToolTip(tr("Same Color Allowed"));
	else
		ui->actionAllow_Same_Colors->setToolTip(tr("Same Color Not Allwed"));

	algorithmsComboBox->setToolTip(tr("Choose the solving algorithm"));
	algorithmsComboBox->setItemText(0, tr("Most Parts"));
	algorithmsComboBox->setItemText(1, tr("Worst Case"));
	algorithmsComboBox->setItemText(2, tr("Expected Size"));

	game->retranslateTexts();
}

void MainWindow::setPegsNumber(const int &pegs_n)
{
	pegsComboBox->setCurrentIndex(pegs_n);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (game->isRunning() && QMessageBox::No == QMessageBox::warning(this,
																	   tr("Quit"), QString(board.isAndroid ? "%1\n%2" : "<p align='center'>%1</p>"
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

void MainWindow::onNewGame()
{
	if (quitUnfinishedGame()) {
		game->stop();
		updateRules();
		ui->actionResign->setEnabled(rules.mode == Rules::Mode::HVM);
		ui->actionResign->setVisible(rules.mode == Rules::Mode::HVM);
		ui->actionReveal_One_Peg->setEnabled(rules.mode == Rules::Mode::HVM);
		ui->actionReveal_One_Peg->setVisible(rules.mode == Rules::Mode::HVM);
		game->play();
	}
}

bool MainWindow::quitUnfinishedGame()
{
	if (!game->isRunning())
		return true;
	int new_game_accept = QMessageBox::warning(this,
								  tr("New Game"), QString(board.isAndroid ? "%1\n%2" : "<p align='center'>%1</p>"
													"<p align='center'>%2</p>")
												.arg(tr("An unfinished game is in progress."))
												.arg(tr("Do you want to start a new game?")),
								  QMessageBox::Yes | QMessageBox::No);
	return (new_game_accept == QMessageBox::Yes);
}

void MainWindow::onFont()
{
	auto preferencesWidget = new Preferences(&board, this);
	preferencesWidget->setModal(true);
	preferencesWidget->setWindowTitle(tr("Font"));
	preferencesWidget->exec();
}

void MainWindow::onQtMindHomePage()
{
	QDesktopServices::openUrl(QUrl("http://omidnikta.github.io/qtmind/index.html", QUrl::TolerantMode));
}

void MainWindow::onAbout()
{
	QStringList app_version = QCoreApplication::applicationVersion().split('.');
	QString localized_app_version = "";
	foreach(QString sub_version_number, app_version) {
		localized_app_version.append(board.locale.toString(sub_version_number.toInt()));
		localized_app_version.append(board.locale.decimalPoint());
	}
	localized_app_version.chop(1);
	QMessageBox::about(this, tr("About QtMind"), QString(board.isAndroid ? "%1 %2\n%3\n\n%4\n%5" :
		"<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>")
		.arg(tr("QtMind"), localized_app_version,
			tr("Code Breaking Game, A Clone Of The Mastermind Board Game"),
			tr("Copyright &copy; 2013-%1 Omid Nikta").arg(board.locale.toString(2014)),
			tr("Released under the <a href=%1>GPL 3</a> license").arg("\"http://www.gnu.org/licenses/gpl.html\"")));
}

void MainWindow::onAutoPutPins()
{
	board.autoPutPins = ui->actionAuto_Set_Pins->isChecked();
}

void MainWindow::onAutoCloseRows()
{
	board.autoCloseRows = ui->actionAuto_Close_Rows->isChecked();
}

void MainWindow::onIndicatorChanged()
{
	board.indicator.showIndicators = ui->menuIndicators->actions().at(0)->isChecked();
	board.indicator.showColors = ui->menuIndicators->actions().at(1)->isChecked();
	game->changeIndicators();
}

void MainWindow::onModeChanged(QAction *selectedAction)
{
	Q_UNUSED(selectedAction);
	if (getMode() != rules.mode)
		onNewGame();
}

void MainWindow::onAlgorithmChanded()
{
	rules.algorithm = static_cast<Rules::Algorithm>(algorithmsComboBox->currentIndex());

}

void MainWindow::onVolumeChanged(QAction *volume_action)
{
	board.sounds.setVolume(volume_action->data().toInt());
}

void MainWindow::onColorActionChanged(QAction *color_action)
{
	if (colorsComboBox->currentIndex() != color_action->data().toInt() - 2) {
		colorsComboBox->setCurrentIndex(color_action->data().toInt() - Rules::MIN_COLOR_NUMBER);
	}
}

void MainWindow::onColorComboChanged(const int &combo_index)
{
	if (ui->menuColors->menuAction()->data().toInt() - 2 != combo_index) {
		ui->menuColors->actions().at(combo_index)->setChecked(true);
	}
}

void MainWindow::onSlotActionChanged(QAction *slot_action)
{
	if (pegsComboBox->currentIndex() != slot_action->data().toInt() - 2) {
		pegsComboBox->setCurrentIndex(slot_action->data().toInt() - Rules::MIN_SLOT_NUMBER);
	}
}

void MainWindow::onSlotComboChanged(const int &combo_index)
{
	if (ui->menuSlots->menuAction()->data().toInt() - 2 != combo_index) {
		ui->menuSlots->actions().at(combo_index)->setChecked(true);
	}
}

void MainWindow::onAlgorithmActionChanged(QAction *algorithm_action)
{
	if (algorithmsComboBox->currentIndex() != algorithm_action->data().toInt()) {
		algorithmsComboBox->setCurrentIndex(algorithm_action->data().toInt());
	}
}

void MainWindow::onAlgorithmComboChanged(const int &combo_index)
{
	if (ui->menuAlgorithm->menuAction()->data().toInt() != combo_index) {
		ui->menuAlgorithm->actions().at(combo_index)->setChecked(true);
	}
}

void MainWindow::onIndicatorTypeChanged(QAction *indic_act)
{
	board.indicator.type = static_cast<Indicator::Type>(indic_act->data().toInt());
	game->changeIndicators();
}

void MainWindow::onLanguageChanged(QAction *language_act)
{
	QLocale newLocale(language_act->data().toString());
	board.locale = newLocale;
	board.locale.setNumberOptions(QLocale::OmitGroupSeparator);
	loadTranslation();
	setLayoutDirection(board.locale.textDirection());
	retranslate();
}

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
	contextMenu.addMenu(ui->menuMode);
	contextMenu.exec(mapToGlobal(position));
}

void MainWindow::updateRules()
{
	rules.mode = getMode();
	rules.colors = colorsComboBox->currentIndex() + Rules::MIN_COLOR_NUMBER;
	rules.pegs = pegsComboBox->currentIndex() + Rules::MIN_SLOT_NUMBER;
	rules.algorithm = static_cast<Rules::Algorithm>(algorithmsComboBox->currentIndex());
	rules.same_colors = (ui->actionAllow_Same_Colors->isChecked()) || (rules.pegs > rules.colors);
	ui->actionAllow_Same_Colors->setChecked(rules.same_colors);
	if (rules.same_colors)
		ui->actionAllow_Same_Colors->setToolTip(tr("Same Color Allowed"));
	else
		ui->actionAllow_Same_Colors->setToolTip(tr("Same Color Not Allwed"));

	// for safety, fallback to standard in out-range inputs
	if (rules.pegs < Rules::MIN_SLOT_NUMBER || rules.pegs > Rules::MAX_SLOT_NUMBER ||
			rules.colors < Rules::MIN_COLOR_NUMBER || rules.pegs > Rules::MAX_COLOR_NUMBER) {
		rules.pegs = 4;
		rules.colors = 6;
	}
}

Rules::Mode MainWindow::getMode()
{
	if (ui->actionMode_MVH->isChecked())
		return Rules::Mode::MVH;
	else	//if (ui->actionMode_HVM->isChecked())
		return Rules::Mode::HVM;
}

QStringList MainWindow::findTranslations()
{
	QStringList result = QDir(AppPath, "*.qm").entryList(QDir::Files);
	result.replaceInStrings(".qm", "");
	return result;
}

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
	QString current = board.locale.name();
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
