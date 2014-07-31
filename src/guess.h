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

#ifndef GUESS_H
#define GUESS_H

#include "rules.h"
#include <QString>

/**
 * @brief A class to represent a guess element.
 *
 */
class Guess
{
public:

	/**
	 * @brief Guess create the guess element
	 */
	Guess();

	/**
	 * @brief reset reset the guess element
	 * @param algorithm_m the new algorithm
	 * @param possibles_m the new possibles number
	 */
	void reset(const Rules::Algorithm &algorithm_m, const int &possibles_m);

private:
	QString guess; /**< TODO */
	QString code; /**< TODO */
	int response; /**< TODO */
	Rules::Algorithm algorithm; /**< TODO */
	int possibles; /**< TODO */
	qreal weight; /**< TODO */

	friend class Solver;
	friend class Game;
};

#endif // GUESS_H
