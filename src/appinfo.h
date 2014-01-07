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

#ifndef APPINFO_H
#define APPINFO_H
#include <QString>

static const int VER_MAJOR			= 0;
static const int VER_MINOR			= 7;
static const int VER_BUGFIX			= 1;
static const QString APP_VER = QString("%1.%2.%3")
		.arg(VER_MAJOR).arg(VER_MINOR).arg(VER_BUGFIX);

static const QString APP_NAME		= "qtmind";
static const QString ORG_NAME		= "qtmind";
static const QString ORG_DOMAIN		= "http://omidnikta.github.io/qtmind";
static const QString AUTHOR_NAME	= "Omid Nikta";

#endif // APPINFO_H
