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

#include "preferences.h"
#include "ui_preferences.h"
#include "boardaid.h"

#include <QFile>
#include <QSettings>
#include <QLibraryInfo>
#include <QDir>
#include <QTranslator>
#include <QSlider>
#include "constants.h"

QString Preferences::AppPath;

//-----------------------------------------------------------------------------

Preferences::Preferences(BoardAid *board_aid, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Preferences),
	boardAid(board_aid)
{
	ui->setupUi(this);
	setLayoutDirection(boardAid->locale.textDirection());

	for(int i = 1; i < 21; ++i)
	{
		ui->fontSizeComboBox->addItem(QString("%1 %2").arg(boardAid->locale.toString(i)).arg(tr("Point(s)", "", i)));
	}
	ui->fontComboBox->setCurrentFont(boardAid->boardFont.fontName);
	ui->fontSizeComboBox->setCurrentIndex(boardAid->boardFont.fontSize);
	ui->volumeVerticalSlider->setValue(boardAid->boardSounds.getVolume());
	ui->showColorsCheckBox->setChecked(boardAid->indicator.showColors);
	ui->characterRadioButton->setChecked(boardAid->indicator.indicatorType == IndicatorType::Character);
	ui->digitRadioButton->setChecked(boardAid->indicator.indicatorType == IndicatorType::Digit);
	ui->languageComboBox->addItem(tr("<System Language>"));

	QStringList translations = findTranslations();
	QString app_name_ = QApplication::applicationName().toLower()+"_";
	translations = translations.filter(app_name_);
	foreach(QString translation, translations)
	{
		translation.remove(app_name_);
		ui->languageComboBox->addItem(languageName(translation), translation);
	}
	int index = qMax(0, ui->languageComboBox->findData(boardAid->locale.name().left(2)));
	ui->languageComboBox->setCurrentIndex(index);

	connect(ui->acceptRejectButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui->acceptRejectButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

}
//-----------------------------------------------------------------------------

Preferences::~Preferences()
{
	delete ui;
}
//-----------------------------------------------------------------------------

void Preferences::loadTranslation(BoardAid *board_aid)
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
	QString current = board_aid->locale.name();
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

QString Preferences::languageName(const QString &language)
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
//-----------------------------------------------------------------------------

QStringList Preferences::findTranslations()
{
	QStringList result = QDir(AppPath, "*.qm").entryList(QDir::Files);
	result.replaceInStrings(".qm", "");
	return result;
}
//-----------------------------------------------------------------------------

void Preferences::accept()
{
	QDialog::accept();
	QLocale newLocale(ui->languageComboBox->itemData(ui->languageComboBox->currentIndex()).toString());
	boardAid->locale = newLocale;
	boardAid->boardFont.fontName = ui->fontComboBox->currentText();
	boardAid->boardFont.fontSize = ui->fontSizeComboBox->currentIndex();
	boardAid->boardSounds.setVolume(ui->volumeVerticalSlider->value());
	boardAid->indicator.showColors = ui->showColorsCheckBox->isChecked();
	boardAid->indicator.indicatorType = ui->digitRadioButton->isChecked() ? IndicatorType::Digit : IndicatorType::Character;
}
