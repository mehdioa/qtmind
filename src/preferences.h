#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
	Q_OBJECT

public:
	explicit Preferences(QLocale* locale_n, QWidget *parent = 0);
	~Preferences();

	static void loadTranslation(const QString& appname);
	static QString languageName(const QString& language);

public slots:
	void accept();

private:
	static QStringList findTranslations();

private:
	Ui::Preferences *ui;

	static QString mCurrent; /**< stored application language */
	static QString mPath; /**< location of translations; found in loadTranslator() */
	static QString mAppName; /**< application name passed to loadTranslator() */

};

#endif // PREFERENCES_H
