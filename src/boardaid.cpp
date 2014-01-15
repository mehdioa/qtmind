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

#include "boardaid.h"
#include "appinfo.h"
#include "QSettings"

const QString BoardAid::underlineAppName = APP_NAME + "_";	//	QtMind_

BoardAid::BoardAid():
	autoPutPins(QSettings().value("AutoPutPins", true).toBool()),
	autoCloseRows(QSettings().value("AutoCloseRows", false).toBool()),
	locale(QSettings().value("Locale/Language", "en").toString().left(5))
{
}

BoardAid::~BoardAid()
{
	QSettings().setValue("AutoPutPins",	autoPutPins);
	QSettings().setValue("AutoCloseRows", autoCloseRows);
	QSettings().setValue("Locale/Language", locale.name());
}
