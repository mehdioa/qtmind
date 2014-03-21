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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

class BoardAid;

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
	Q_OBJECT

public:
	explicit Preferences(BoardAid *board_aid, QWidget *parent = 0);
	~Preferences();

public slots:
	void accept();

private:
	Ui::Preferences *ui;

	static QString AppPath; /**< location of translations; found in loadTranslator() */
	BoardAid *boardAid;
};

#endif // PREFERENCES_H
