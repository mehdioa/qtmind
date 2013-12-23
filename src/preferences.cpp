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

#include <QFile>
#include <QSettings>
#include <QLibraryInfo>
#include <QDir>
#include <QTranslator>
#include <QSlider>
#include "constants.h"
//-----------------------------------------------------------------------------

QString Preferences::mCurrent;
QString Preferences::mPath;
QString Preferences::mAppName;

//-----------------------------------------------------------------------------

Preferences::Preferences(QLocale *locale_n, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Preferences)
{
	ui->setupUi(this);
	setLayoutDirection(locale_n->textDirection());

	for(int i = 1; i < 21; ++i)
	{
		ui->fontSizeComboBox->addItem(QString("%1 %2").arg(locale_n->toString(i)).arg(tr("Points")));
	}
	ui->fontSizeComboBox->setCurrentIndex(QSettings().value("FontSize", 12).toInt());
	ui->fontComboBox->setCurrentFont(QFont(QSettings().value("FontName", "Sans Serif").toString()));
	ui->volumeVerticalSlider->setValue(QSettings().value("Volume", 50).toInt());
	ui->showColorsCheckBox->setChecked(QSettings().value("ShowColors", 1).toBool());
	ui->characterRadioButton->setChecked((IndicatorType) QSettings().value("IndicatorType", 0).toInt() == IndicatorType::Character);
	ui->digitRadioButton->setChecked((IndicatorType) QSettings().value("IndicatorType", 0).toInt() == IndicatorType::Digit);
	ui->noteTextLabel->setText(tr("Language change needs restart."));
	ui->languageComboBox->addItem(tr("<System Language>"));

	QString translation;
	QStringList translations = findTranslations();
	foreach (translation, translations) {
		translation.remove(mAppName);
		ui->languageComboBox->addItem(languageName(translation), translation);
	}
	int index = qMax(0, ui->languageComboBox->findData(mCurrent));
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

void Preferences::loadTranslation(const QString &name)
{
	QString appdir = QCoreApplication::applicationDirPath();
	mAppName = name;

	// Find translator path
	QStringList paths;
	paths.append(appdir + "/translations/");
	paths.append(appdir + "/../share/" + QCoreApplication::applicationName().toLower() + "/translations/");
	paths.append(appdir + "/../Resources/translations");
	foreach (const QString &path, paths) {
		if (QFile::exists(path)) {
			mPath = path;
			break;
		}
	}

	// Find current locale
	mCurrent = QSettings().value("Locale/Language").toString();
	QString current = !mCurrent.isEmpty() ? mCurrent : QLocale::system().name();
	QStringList translations = findTranslations();
	if (!translations.contains(mAppName + current)) {
		current = current.left(2);
		if (!translations.contains(mAppName + current)) {
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
		qt_translator.load("qt_" + current, mPath);
	} else {
		qt_translator.load("qt_" + current, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	}
	QCoreApplication::installTranslator(&qt_translator);

	static QTranslator translator;
	translator.load(mAppName + current, mPath);
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
	QStringList result = QDir(mPath, "*.qm").entryList(QDir::Files);
	result.replaceInStrings(".qm", "");
	return result;
}
//-----------------------------------------------------------------------------

void Preferences::accept()
{
	QDialog::accept();
	mCurrent = ui->languageComboBox->itemData(ui->languageComboBox->currentIndex()).toString();
	QSettings().setValue("Locale/Language", mCurrent);
	QSettings().setValue("FontName", ui->fontComboBox->currentText());
	QSettings().setValue("FontSize", ui->fontSizeComboBox->currentIndex());
	QSettings().setValue("Volume", ui->volumeVerticalSlider->value());
	QSettings().setValue("ShowColors", (int) ui->showColorsCheckBox->isChecked());
	QSettings().setValue("IndicatorType", (int) ui->digitRadioButton->isChecked());
}
