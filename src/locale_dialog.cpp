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

#include "locale_dialog.h"

#include <QComboBox>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QSettings>
#include <QTranslator>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------

QString LocaleDialog::mCurrent;
QString LocaleDialog::mPath;
QString LocaleDialog::mAppName;

//-----------------------------------------------------------------------------

LocaleDialog::LocaleDialog(QWidget* parent) :
	QDialog(parent, Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
	QString title = parent ? parent->window()->windowTitle() : QString();
	setWindowTitle(!title.isEmpty() ? title : QCoreApplication::applicationName());

	QLabel* text = new QLabel(tr("Select application language:"), this);

	mTranslations = new QComboBox(this);
	mTranslations->addItem(tr("<System Language>"));
	QString translation;
	QStringList translations = findTranslations();
	foreach (translation, translations) {
		if (translation.startsWith("qt")) {
			continue;
		}
		translation.remove(mAppName);
		mTranslations->addItem(languageName(translation), translation);
	}
	int index = qMax(0, mTranslations->findData(mCurrent));
	mTranslations->setCurrentIndex(index);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSizeConstraint(QLayout::SetFixedSize);
	layout->addWidget(text);
	layout->addWidget(mTranslations);
	layout->addWidget(buttons);
}

//-----------------------------------------------------------------------------

void LocaleDialog::loadTranslation(const QString& name)
{
	QString appdir = QCoreApplication::applicationDirPath();
	mAppName = name;

	// Find translator path
	QStringList paths;
	paths.append(appdir + "/translations/");
	paths.append(appdir + "/../share/" + QCoreApplication::applicationName().toLower() + "/translations/");
	paths.append(appdir + "/../Resources/translations");
	foreach (const QString& path, paths) {
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

QString LocaleDialog::languageName(const QString& language)
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

QStringList LocaleDialog::findTranslations()
{
	QStringList result = QDir(mPath, "*.qm").entryList(QDir::Files);
	result.replaceInStrings(".qm", "");
	return result;
}

//-----------------------------------------------------------------------------

void LocaleDialog::accept()
{
	int current = mTranslations->findData(mCurrent);
	if (current == mTranslations->currentIndex()) {
		return reject();
	}
	QDialog::accept();

	mCurrent = mTranslations->itemData(mTranslations->currentIndex()).toString();
	QSettings().setValue("Locale/Language", mCurrent);
	QMessageBox::information(this, tr("Note"), tr("Please restart this application for the change in language to take effect."), QMessageBox::Ok);
}

//-----------------------------------------------------------------------------
