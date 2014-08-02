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

#ifndef RULES_H
#define RULES_H

#include <QLocale>

/**
 * @brief The Rules class represents the rules of the game
 */
class Rules
{

public:

	/**
	 * @brief The Algorithm enum the solving algorithm
	 */
	enum class Algorithm
	{
		MostParts,
		WorstCase,
		ExpectedSize
	};
	static const int MIN_COLOR_NUMBER	= 2; /**< The minimum number of colors */
	static const int MAX_COLOR_NUMBER	= 10; /**< The maximum number of colors */

	static const int MIN_SLOT_NUMBER	= 2; /**< The minimum number of slots */
	static const int MAX_SLOT_NUMBER	= 5; /**< The maximum number of slots */
	/**
	 * @brief Rules
	 */
	Rules();
	~Rules();

private:

	enum Mode {
		MVH,		//HUMAN MAKE THE HIDDEN CODE
		HVM		    //HUMAN BREAKS THE HIDDEN CODE
	};

	int pegs; /**< TODO */
	int colors; /**< TODO */
	bool same_colors; /**< TODO */
	Rules::Algorithm algorithm; /**< TODO */
	Mode mode; /**< TODO */

	friend class MainWindow;
	friend class Solver;
	friend class Game;
	friend class PinBox;
};

#endif // RULES_H
