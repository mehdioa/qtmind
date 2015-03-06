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

    explicit Guess();
    ~Guess(){}

public:
    /**
     * @brief setGuess set the guess
     * @param _guess the guesss
     */
    void setGuess(unsigned char* guess);
    /**
     * @brief setCode set the code
     * @param _code the code
     */
    void setCode(unsigned char* code);
    /**
     * @brief update sets mBlacks, mWhites, and mPossibles
     * @param b blacks
     * @param w whites
     * @param p possibles
     */
    void update(const int& b, const int& w, const int& p);
    /**
     * @brief reset reset the guess element
     * @param colors the new colors
     * @param pegs the  new pegs
     * @param algorithm the new algorithm
     * @param possibles the new possibles number
     */
    void reset(const int& colors, const int& pegs, const Algorithm& algorithm, const int& possibles);
    /**
     * @brief setWeight sets the weight
     * @param weight the new weight
     */
    void setWeight(const qreal& weight);
private:
    unsigned char mGuess[MAX_SLOT_NUMBER]; /**< TODO */
    unsigned char mCode[MAX_SLOT_NUMBER]; /**< TODO */
    int mColors;
    int mPegs;
    int mBlacks;
    int mWhites;
    Algorithm mAlgorithm; /**< TODO */
    int mPossibles; /**< TODO */
    qreal mWeight; /**< TODO */

    friend class Game;
};

#endif // GUESS_H
