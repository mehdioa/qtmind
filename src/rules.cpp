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

#include "rules.h"
#include <QSettings>

Rules::Rules():
	pegs(QSettings().value("Pegs", 4).toInt()),
	colors(QSettings().value("Colors", 6).toInt()),
	same_colors(QSettings().value("SameColor", true).toBool()),
	algorithm((Rules::Algorithm) QSettings().value("Algorithm", 0).toInt()),
	mode((Mode) QSettings().value("Mode", 1).toInt())
{

}

Rules::~Rules()
{
	QSettings().setValue("Pegs", pegs);
	QSettings().setValue("Colors", colors);
	QSettings().setValue("SameColor", same_colors);
	QSettings().setValue("Algorithm", (int) algorithm);
	QSettings().setValue("Mode", (int) mode);
}
