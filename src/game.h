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

#ifndef GAME_H
#define GAME_H

#include <QList>
#include <QString>
#include "constants.h"

class Game
{
	/*	The class Game is the engine of the mastermind game. It contains all the solving
	 *	algorithms and auxiliary functions that provide efficient code guess and handling
	 *	response and so on. The response is stored this way:
	 *	example: p := peg number = 4
	 *	(black, white)
	 *
	 *	(0, 4)
	 *
	 *	(0, 3)	(1, 3)
	 *
	 *	(0, 2)	(1, 2)	(2, 2)
	 *
	 *	(0, 1)	(1, 1)	(2, 1)	(3, 1)
	 *
	 *	(0, 0)	(1, 0)	(2, 0)	(3, 0)	(4, 0)
	 *
	 *	There are 1+2+...+(p+1) = (p+1)(p+2)/2 couples (including the impossible (3, 1)).
	 *	The one-to-one function f: Set of possible couples (black, white) --> [0...(p+1)(p+2)/2 - 1]
	 *	including (3, 1) is defined
	 *
	 *			f(b, w) = (b+w)(b+w+1)/2 + b;
	 */
public:
	explicit Game(const int& peg_no, const int& color_no,
		 const bool& allow_same_color);
	~Game();
	bool done () const {return (mResponse == mResponseSpaceSize - 1);}
	QString getInformation() const {return mInformation;}
	QString getGuess();
	QString getGuess(const Algorithm& alg);
	bool setResponse(const int& response);
	void reset(const int& peg_no, const int& color_no,
			   const bool& allow_same_color);

private:
	void createTables();
	void deleteTables();
	int compare (const int *codeA, const int *codeB) const;
	QString arrayToString(const int*) const;
	void stringToArray(const QString& str, int* arr) const;
	int computeWeight(int* responses, const Algorithm& alg) const;
	void convertBase(int decimal, const int &base,
					 const int &precision, int* convertedArray);


	int mPegNumber;										//	pegs count, 4
	int mColorNumber;									//	colors count, 6
	bool mAllowSameColor;
	QString mGuess;										//	the guess number, ?
	int mResponse;										//	the black-white response code, [0..14]
	int mResponseSpaceSize;
	int mAllCodesSize;									//	the size of the complete code space, 6^4 = 1296
	QString mInformation;

	int** mAllCodes;									//	all indexes of codes (0...1295)
	QList<int> mPossibleCodes;							//	list of all possibles
	int mFirstPossibleCodesSize;
	int* mFirstPossibleCodes;							//	Contains the first remaining possibles (in case mAllCodesSize > 10000) or is mAllCodes otherwise

};

#endif // GAME_H
