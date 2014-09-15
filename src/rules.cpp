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

#include "appinfo.h"
#include "rules.h"
#include <QSettings>

Rules::Rules():
	m_pegs(QSettings().value("Pegs", 4).toInt()),
	m_colors(QSettings().value("Colors", 6).toInt()),
	m_sameColors(QSettings().value("SameColor", true).toBool()),
	m_algorithm((Algorithm) QSettings().value("Algorithm", 0).toInt()),
	m_mode((Mode) QSettings().value("Mode", 1).toInt())
{

}

Rules *Rules::instance()
{
	static Rules rules;
	return &rules;
}

Rules::~Rules()
{
	QSettings().setValue("Pegs", m_pegs);
	QSettings().setValue("Colors", m_colors);
	QSettings().setValue("SameColor", m_sameColors);
	QSettings().setValue("Algorithm", (int) m_algorithm);
	QSettings().setValue("Mode", (int) m_mode);
}
