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

#include "guess.h"
#include <QDebug>
#include <QMutex>

Guess* Guess::sGuess = 0;

Guess::Guess()
{
	reset(0);
}
qreal Guess::weight() const
{
    return mWeight;
}

void Guess::setWeight(const qreal &w)
{
    mWeight = w;
}

unsigned char Guess::guess(const int &index) const
{
    return mGuess[index];
}

const unsigned char *Guess::guess() const
{
    return mGuess;
}

void Guess::update(const int &b, const int &w, const int &p)
{
    mBlacks = b;
    mWhites = w;
    mPossibles = p;
}

int Guess::possibles() const
{
    return mPossibles;
}

int Guess::whites() const
{
    return mWhites;
}

int Guess::blacks() const
{
    return mBlacks;
}

Algorithm Guess::algorithm() const
{
    return mAlgorithm;
}

void Guess::setAlgorithm(const Algorithm &algorithm)
{
    mAlgorithm = algorithm;
}


Guess *Guess::instance()
{
    static QMutex mutex;
    if (!sGuess)
    {
        mutex.lock();

        if (!sGuess)
            sGuess = new Guess;

        mutex.unlock();
    }
    return sGuess;
}

void Guess::reset(const int &_possibles)
{
    mColors = Rules::instance()->colors();
    mPegs = Rules::instance()->pegs();
    std::fill(mGuess, mGuess + MAX_SLOT_NUMBER, 0);
    std::fill(mCode, mCode + MAX_SLOT_NUMBER, 0);
    mBlacks = 0;
    mWhites = 0;
    mAlgorithm = Rules::instance()->algorithm();
    mPossibles = _possibles;
    mWeight = 0;
}

void Guess::setGuess(unsigned char *_guess)
{
	for(int i = 0; i < MAX_SLOT_NUMBER; i++)
        mGuess[i] = _guess[i];
    COMPARE(mCode, mGuess, mColors, mPegs, mBlacks, mWhites);
}

void Guess::setCode(unsigned char *_code)
{
	for(int i = 0; i < MAX_SLOT_NUMBER; i++)
        mCode[i] = _code[i];
}
