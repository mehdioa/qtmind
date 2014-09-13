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
#include "appinfo.h"
#include "rules.h"
#include <QString>

/**
  * @brief compares two codes A and B, which are two int arrays
  * C and P are the number of colors and pegs
  *
  * Interestingly, total can be computed as
  * f[total := whites + blacks = ( \sum_{i=1}^6 \min(c_i, g_i)) \f]
  * where c_i is the number of times the color i is in the code and g_i is the number of times it is in the guess. See
  * http://mathworld.wolfram.com/Mastermind.html
  * for more information
  */
#define COMPARE(A, B, C, P, BL, WT) {\
	BL = 0;\
	int __c[C], __g[C];\
	std::fill(__c, __c+C, 0);\
	std::fill(__g, __g+C, 0);\
	for (int i = 0; i < P; ++i) {\
		if (A[i] == B[i])\
			++BL;\
		++__c[A[i]];\
		++__g[B[i]];\
	}\
	WT = (-BL);\
	for(int i = 0; i < C; ++i)\
		WT += (__c[i] < __g[i]) ? __c[i] : __g[i];\
}

/**
 * @brief A class to represent a guess element.
 *
 */
class Guess
{
public:

	static Guess *instance();
	/**
	 * @brief reset reset the guess element
	 * @param algorithm_m the new algorithm
	 * @param possibles_m the new possibles number
	 */
	void reset(const int &possibles_m);

	/**
	 * @brief setGuess
	 * @param _guess
	 */
	void setGuess(unsigned char *_guess);

	/**
	 * @brief setCode
	 * @param _code
	 */
	void setCode(unsigned char *_code);

	int getBlacks() const { return blacks; }
	int getWhites() const { return whites; }
private:
	/**
	 * @brief Guess create the guess element
	 */
	explicit Guess();

private:
	unsigned char guess[MAX_SLOT_NUMBER]; /**< TODO */
	unsigned char code[MAX_SLOT_NUMBER]; /**< TODO */
	int colors;
	int pegs;
	int blacks;
	int whites;
	Algorithm algorithm; /**< TODO */
	int possibles; /**< TODO */
	qreal weight; /**< TODO */

	friend class Solver;
	friend class Game;
};

#endif // GUESS_H
