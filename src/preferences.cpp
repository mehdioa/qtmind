#include "preferences.h"
#include "ui_preferences.h"

#include <QFile>
#include <QSettings>
#include <QLibraryInfo>
#include <QDir>
#include <QTranslator>
#include <QMessageBox>
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

	ui->languageComboBox->addItem(tr("<System Language>"));

	QString translation;
	QStringList translations = findTranslations();
	foreach (translation, translations) {
		if (translation.startsWith("qt")) {
			continue;
		}
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

void Preferences::loadTranslation(const QString& name)
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

QString Preferences::languageName(const QString& language)
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
	QMessageBox::information(this, tr("Note"), tr("Changes will take effect next time."), QMessageBox::Ok);
}

//-----------------------------------------------------------------------------