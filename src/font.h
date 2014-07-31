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

#ifndef FONT_H
#define FONT_H

#include <QString>

/**
 * @brief The Font class represent the font that will be used in the board
 */
class Font
{
public:

	/**
	 * @brief Font read settings and create a font
	 */
	Font();
	~Font();

private:
	QString name; /**< TODO */
	int size; /**< TODO */

	friend class Button;
	friend class Message;
	friend class Board;
	friend class Preferences;
	friend class MainWindow;
};


#endif // FONT_H
