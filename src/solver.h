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

#ifndef SOLVER_H
#define SOLVER_H

#include "constants.h"
#include <QList>
#include <QString>
#include <QThread>

/*	The class Solver is the engine of the mastermind game. It contains all the solving
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

class Solver : public QThread
{
	Q_OBJECT

public:
	explicit Solver(const int &peg_no, const int &color_no,
		 const bool &allow_same_color, QObject *parent = 0);
	~Solver();
	bool done () const {return (mResponse == mResponseSpaceSize - 1);}
	bool setResponse(const int &response);
	void run();

protected slots:
	void onInterupt() {mInterupt = true;}
	void onStartGuessing(const Algorithm &alg);
	void onReset(const int &peg_no, const int &color_no,
			   const bool &allow_same_color);

signals:
	void guessDoneSignal(Algorithm, QString guess, int possible_size, qreal min_weight);

private:
	void makeGuess();
	QString getFirstGuess() const;
	QString randomPermutation(QString str) const;
	void permute(QString &code) const;
	void createTables();
	void deleteTables();
	int compare (const int *codeA, const int *codeB) const;
	QString arrayToString(const int*) const;
	void stringToArray(const QString &str, int *arr) const;
	qreal computeWeight(int *responses) const;
	void convertBase(int decimal, const int &base,
					 const int &precision, int *convertedArray);

private:
	int mPegNumber;										//	pegs count, 4
	int mColorNumber;									//	colors count, 6
	bool mAllowSameColor;
	Algorithm mAlgorithm;
	QString mGuess;										//	the guess number, ?
	int mResponse;										//	the black-white response code, [0..14]
	int mResponseSpaceSize;
	int mAllCodesSize;									//	the size of the complete code space, 6^4 = 1296
	qreal mLastMinWeight;
	volatile bool mInterupt;

	int **mAllCodes;									//	all indexes of codes (0...1295)
	int *mFirstPossibleCodes;							//	Contains the first remaining possibles (in case mAllCodesSize > 10000) or is mAllCodes otherwise
	int mFirstPossibleCodesSize;
	QList<int> mPossibleCodes;							//	list of all possibles

};

#endif // SOLVER_H
