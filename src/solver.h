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

#include <QList>
#include <QThread>

/**	@brief The class Solver is the solving engine of the mastermind game. It contains all the solving
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
 *	There are \f$ 1+2+...+(p+1) = (p+1)(p+2)/2 \f$ couples (including the impossible \f$ (3, 1)=(p-1,1)\f$). If
 *          \f[ X = \{(b,w): \,\,\, 0 \leq b,\, 0\leq w\,\, \wedge\,\,  b+w\leq p\} \f]
 *	The one-to-one function \f$ f: X \to [0...(p+1)(p+2)/2 - 1]\f$
 *	is defined
 *			\f[ f(b, w) = (b+w)(b+w+1)/2 + b \f]
 *
 *
 */
class Solver : public QThread
{
	Q_OBJECT

public:
	/**
	 * @brief a helper function to find the power of an integer by another integer
	 *
	 * @param base the base of the power base^exp
	 * @param exp the power
	 * @return int the power base^exp
	 */
	static int ipow(int base, int exp);
	/**
	 * @brief Solver
	 * @param guess_element
	 * @param parent
	 */
	explicit Solver(QObject *parent = 0);
	~Solver();

	/**
	 * @brief check if the response is valid and remove impossibles
	 * @param m_response
	 * @return return true if the response is valid, false otherwise
	 */
	bool setResponse(const int &blacks, const int &whites);

	/**
	 * @brief
	 *
	 */
	void run();

protected slots:

	/**
	 * @brief
	 *
	 */
	void onInterupt() {interupt = true;}

	/**
	 * @brief
	 *
	 */
	void onStartGuessing();

	/**
	 * @brief
	 *
	 */
	void onReset();

signals:

	/**
	 * @brief
	 *
	 */
	void guessDoneSignal();

private:

	void makeGuess();

	/**
	 * @brief Use Knuth's shuffling method to shuffle a string
	 *
	 * @param m_string the shuffling string
	 */
	void shuffle(unsigned char *m_string, int len) const;

	/**
	 * @brief permute a code
	 *
	 * @param m_code permutting code
	 */
	void permute(unsigned char *m_code) const;

	/**
	 * @brief create internal tables
	 *
	 */
	void createTables();

	/**
	 * @brief delete internal tables and release memory
	 *
	 */
	void deleteTables();

	/**
	 * @brief
	 *
	 * @param m_array
	 * @return QString
	 */
	QString arrayToString(const unsigned char *m_array) const;

	/**
	 * @brief
	 *
	 * @param m_responses
	 * @return qreal
	 */
	qreal computeWeight(int *m_responses) const;

	/**
	 * @brief
	 *
	 * @param number
	 */
	void indexToCodeSameColor(unsigned char *X);

	/**
	 * @brief indexToCodeDifferentColor
	 * @param number
	 */
	void indexToCodeDifferentColor(unsigned char *X);

	/**
	 * @brief
	 *
	 */
	void setSmallPossibles();

private:

	/**
	 * @brief The Codes struct
	 * All codes
	 */
	struct Codes {
		int size;
		unsigned char **index;
	} codes;

	/**
	 * @brief The FirstPossiblesUnder10_000 struct
	 * The first possibles codes under ten thousands
	 */
	struct FirstPossiblesUnder10_000 {
		int size;
		int *index;
	} sp;

	int max_response; /**< TODO */
	volatile bool interupt; /**< TODO */
	QList<int> possibles;   //	list of all possibles
};

#endif // SOLVER_H
