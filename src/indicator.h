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

#ifndef INDICATOR_H
#define INDICATOR_H

/**
 * @brief A class to represent the indicator of the board
 *
 */
class Indicator
{
public:
	/**
	 * @brief The Type enum represents the type of the indicator.
	 */
	enum Type {
		Character,
		Digit
	};

	/**
	 * @brief Indicator
	 */
	Indicator();
	~Indicator();

	/**
	 * @brief forceColor check if color should be used
	 * @return true if the colors should be used, false otherwise
	 */
	bool forceColor() const;

private:
	bool showColors; /**< TODO */
	bool showIndicators; /**< TODO */
	Type type; /**< TODO */

	friend class MainWindow;
	friend class Preferences;
	friend class Peg;
};

#endif // INDICATOR_H
