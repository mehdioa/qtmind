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

#ifndef LOCALE_DIALOG_H
#define LOCALE_DIALOG_H

#include <QDialog>
class QComboBox;

class LocaleDialog : public QDialog
{
	Q_OBJECT

public:
	LocaleDialog(QWidget* parent = 0);
	static void loadTranslation(const QString& appname);
	static QString languageName(const QString& language);

public slots:
	void accept();

private:
	static QStringList findTranslations();

private:
	QComboBox* mTranslations; /**< list of found translations */

	static QString mCurrent; /**< stored application language */
	static QString mPath; /**< location of translations; found in loadTranslator() */
	static QString mAppName; /**< application name passed to loadTranslator() */
};

#endif // LOCALE_DIALOG_H
