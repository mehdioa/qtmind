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

#ifndef BOARD_H
#define BOARD_H

#include "indicator.h"
#include "sounds.h"
#include "font.h"
#include <QLocale>

/**
 * @brief A class to represent board tools, like font, locale, indicator.
 */
class Board
{
public:
	/**
	 * @brief Creates an object by reading settings.
	 */
	Board();
	~Board();

private:
	static const bool isAndroid; /**< Is the platform Android? */
	bool autoPutPins; /**< TODO */
	bool autoCloseRows; /**< TODO */
	Indicator indicator; /**< TODO */
	QLocale locale; /**< TODO */
	Font font; /**< TODO */
	Sounds sounds; /**< TODO */

	friend class MainWindow;
	friend class Preferences;
	friend class Game;
};

#endif // BOARD_H
