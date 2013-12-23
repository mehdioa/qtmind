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
	bool done () const {return (response == responseSpaceSize - 1);}
	bool setResponse(const int &m_response);
	void run();

protected slots:
	void onInterupt() {interupt = true;}
	void onStartGuessing(const Algorithm &m_algorithm);
	void onReset(const int &peg_no, const int &color_no,
			   const bool &allow_same_color);

signals:
	void guessDoneSignal(Algorithm m_algorithm, QString m_guess, int possible_size, qreal min_weight);

private:
	void makeGuess();
	QString getFirstGuess() const;
	QString shuffle(QString m_string) const;
	void permute(QString &m_code) const;
	void createTables();
	void deleteTables();
	int compare (const int *codeA, const int *codeB) const;
	QString arrayToString(const int*) const;
	void stringToArray(const QString &m_string, int *m_array) const;
	qreal computeWeight(int *m_responses) const;
	void convertBase(int m_decimal, const int &m_base,
					 const int &m_precision, int *m_convertedArray);

private:
	int pegNumber;										//	pegs count, 4
	int colorNumber;									//	colors count, 6
	bool allowSameColor;
	Algorithm algorithm;
	QString guess;										//	the guess number, ?
	int response;										//	the black-white response code, [0..14]
	int responseSpaceSize;
	int allCodesSize;									//	the size of the complete code space, 6^4 = 1296
	qreal lastMinWeight;
	volatile bool interupt;

	int **allCodes;									//	all indexes of codes (0...1295)
	int *firstPossibleCodes;							//	Contains the first remaining possibles (in case mAllCodesSize > 10000) or is mAllCodes otherwise
	int firstPossibleCodesSize;
	QList<int> possibleCodes;							//	list of all possibles

};

#endif // SOLVER_H
