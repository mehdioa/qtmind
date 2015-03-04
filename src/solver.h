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
#include "appinfo.h"
class Guess;

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
	 * @param base the base of the power base^exp
	 * @param exp the power
	 * @return int the power base^exp
	 */
	static int ipow(int base, int exp);

    explicit Solver(Guess *guess, QObject *parent = 0);

    ~Solver();

	/**
	 * @brief check if the response is valid and remove impossibles
     * @param blacks the number of blacks
     * @param whites the number of whites
	 * @return return true if the response is valid, false otherwise
	 */
    bool setResponse(const int &blacks, const int &whites);

	/**
     * @brief run method of the thread
	 *
	 */
	void run();
	/**
     * @brief used to interupt the guess process which is running in another thread
	 *
	 */
    void interupt() {mInterupt = true;}
    /**
     * @brief reset reset the solver
     * @param colors the number of colors
     * @param pegs the number of pegs
     * @param same_colors same color allowed flag
     * @return
     */
    int reset(const int &colors, const int &pegs, const bool &same_colors);
    /**
     * @brief startGuessing start the guessing process
     * @param alg the guessing algorithm
     */
    void startGuessing(const Algorithm &alg);

signals:

	/**
     * @brief the guess done signal, to be emited when the process of guessing finished
	 */
	void guessDoneSignal();

private:
    /**
     * @brief makeGuess make the guess
     */
    void makeGuess();
	/**
	 * @brief Use Knuth's shuffling method to shuffle a string
	 * @param m_string the shuffling string
	 */
	void shuffle(unsigned char *m_string, int len) const;
	/**
	 * @brief permute a code
	 * @param m_code permutting code
	 */
	void permute(unsigned char *m_code) const;
	/**
	 * @brief create internal tables
	 */
	void createTables();
	/**
	 * @brief delete internal tables and release memory
	 */
	void deleteTables();
	/**
     * @brief turn an array of characters to a QString
     * @param m_array the array of chars
     * @return QString the QString of the m_array
	 */
	QString arrayToString(const unsigned char *m_array) const;
	/**
     * @brief compute the weight of a possible response
     * @param m_responses the response to be weighted
     * @return qreal the weight of the response
	 */
	qreal computeWeight(int *m_responses) const;
	/**
     * @brief find the next code of a code when same color is allowed
     * @param X the code
	 */
    void nextCodeSameColor(unsigned char *X);
    /**
     * @brief find the next code of a code when same color is not allowed
     * @param X the code
     */
    void nextCodeDifferentColor(unsigned char *X);
	/**
     * @brief set the small set of possibles under 10_000
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
    } mCodes;

	/**
	 * @brief The FirstPossiblesUnder10_000 struct
	 * The first possibles codes under ten thousands
	 */
	struct FirstPossiblesUnder10_000 {
		int size;
		int *index;
    } mSmallPossibles;

    int mPegs; /**< the number of pegs */
    int mColors; /**< the number of colors */
    bool mSameColors; /**< same color allowed flag */
    Algorithm mAlgorithm; /**< the solving algorithm */
    int mMaxResponse; /**< maximum number of responses */
    volatile bool mInterupt; /**< the interupt flag */
    QList<int> mPossibles;   /**<	list of all possibles */
    Guess *mGuess; /**< the guess element */
};

#endif // SOLVER_H
