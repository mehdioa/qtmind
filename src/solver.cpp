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

#include "solver.h"
#include "guess.h"
#include "ctime"
#include <QtCore/qmath.h>
#include <stdlib.h>
#include <QDebug>


int Solver::ipow(int base, int exp) {
	int result = 1;
	while (exp) {
		if (exp & 1)
			result *= base;
		exp >>= 1;
		base *= base;
	}
	return result;
}

Solver::Solver(Guess *guess, QObject *parent):
	QThread(parent),
    mInterupt(true),
    mGuess(guess)
{
    mCodes.index = NULL;
    mSmallPossibles.index = NULL;
}

Solver::~Solver()
{
	deleteTables();
}

inline static void array_copy(unsigned char *A, unsigned char *B, int N) {
	for (int i = 0; i < N; i++)
		B[i] = A[i];
}

void Solver::createTables()
{
    if (mSameColors) {
        mCodes.size = ipow(mColors, mPegs);
	} else {
        mCodes.size = 1;
        for(int i = 0; i < mPegs; ++i)
            mCodes.size *= (mColors - i);
	}

    mMaxResponse = (mPegs + 1)*(mPegs + 2)/2;

    mCodes.index = new unsigned char *[mCodes.size];

    for(int i = 0; i < mCodes.size; ++i) {
        mCodes.index[i] = new unsigned char[mPegs];
	}

    if (mSameColors) {
        for (int i = 0; i < mPegs; i++)
            mCodes.index[0][i] = 0;
        for (int i = 1; i < mCodes.size; i++) {
            array_copy(mCodes.index[i-1], mCodes.index[i], mPegs);
            nextCodeSameColor(mCodes.index[i]);
		}
	} else {
        for (int i = 0; i < mPegs; i++)
            mCodes.index[0][i] = i;
        for (int i = 1; i < mCodes.size; i++) {
            array_copy(mCodes.index[i-1], mCodes.index[i], mPegs);
            nextCodeDifferentColor(mCodes.index[i]);
		}
	}

    for(int i = 0; i < mCodes.size; ++i)
        mPossibles.append(i);
}

void Solver::deleteTables()
{
    if (mCodes.index != NULL) {
        for(int i = 0; i < mCodes.size; ++i)
            delete[] mCodes.index[i];

        delete[] mCodes.index;
        mCodes.index = NULL;
	}

    if(mSmallPossibles.index) {
        delete[] mSmallPossibles.index;
        mSmallPossibles.index = NULL;
	}

    mPossibles.clear();
}

int Solver::reset(const int &colors, const int &pegs, const bool &same_colors)
{
    mColors = colors;
    mPegs = pegs;
    mSameColors = same_colors;
	deleteTables();
	createTables();
    return mCodes.size;
}

void Solver::startGuessing(const Algorithm &alg)
{
    mInterupt = false;
    mAlgorithm = alg; // to prevent change in algorithm by user in the middle of computation
	start(QThread::NormalPriority);
}

void Solver::shuffle(unsigned char *m_string, int len) const
{
	qsrand(time(NULL));
	for (int i = 0; i < len; i++) {
		int j = static_cast<int>(i *(qrand()/(RAND_MAX + 1.0)));
		char c = m_string[i];
		m_string[i] = m_string[j];
		m_string[j] = c;
	}
}

void Solver::permute(unsigned char *m_code) const
{
	unsigned char shuffled_colors[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255};
    shuffle(shuffled_colors, mColors);
    shuffle(m_code, mPegs);

    for(int i = 0; i < mPegs; ++i)
		m_code[i] = shuffled_colors[m_code[i]];
}

bool Solver::setResponse(const int &blacks, const int &whites)
{
	QList<int> temppossibles;
	int _blacks, _whites;
    const unsigned char *guess = mGuess->mGuess;
    foreach(int possible, mPossibles) {
        COMPARE(guess, mCodes.index[possible], mColors, mPegs, _blacks, _whites);
		if (blacks == _blacks && whites == _whites)
			temppossibles.append(possible);
	}

	if (temppossibles.isEmpty())
		return false;
    mPossibles = temppossibles;
    mGuess->update(blacks, whites, mPossibles.size());
	setSmallPossibles();
	return true;
}

void Solver::setSmallPossibles()
{
    if (!mSmallPossibles.index)
	{
        if (mCodes.size <= 10000) {
            mSmallPossibles.size = mCodes.size;
            mSmallPossibles.index = new int[mSmallPossibles.size];
            for(int i = 0; i < mSmallPossibles.size; ++i)
                mSmallPossibles.index[i] = i;
        } else if (mPossibles.size() <= 10000) {
            mSmallPossibles.size = mPossibles.size();
            mSmallPossibles.index = new int[mSmallPossibles.size];
            for(int i = 0; i < mSmallPossibles.size; ++i)
                mSmallPossibles.index[i] = mPossibles.at(i);
		}
	}
}

void Solver::run()
{
	makeGuess();
    if (!mInterupt)
		emit guessDoneSignal();
}

void Solver::makeGuess()
{
	// The first guess here
    if (mPossibles.size() == mCodes.size) {
		unsigned char answer[] = {0, 1, 2, 3, 4};
        if (mSameColors) {
            switch (mColors) {
			case 2:
				answer[2] = 0;
				answer[3] = 1;
				answer[2] = 0;
				break;
			case 3:
				answer[3] = 1;
				answer[4] = 2;
				break;
			default:
				answer[3] = 2;
				answer[4] = 3;
				break;
			}
			// the classic game (c = 6, p = 4) is best with this first guess on Worst Case
            if(mColors == 6 && mPegs == 4 && mAlgorithm == Algorithm::WorstCase) {
				answer[2] = 0;
				answer[3] = 1;
			}
		}
		permute(answer);
        mGuess->setGuess(answer);
		return;
	}

    if (mPossibles.size() == 1) {
        mGuess->setGuess(mCodes.index[mPossibles.first()]);
		return;
	}

    if(mPossibles.size() > 10000) {
        mGuess->setGuess(mCodes.index[mPossibles.at(mPossibles.size() >> 1)]);
		return;
	}

    int responsesOfCodes[mMaxResponse];
    for(int i = 0; i < mMaxResponse; ++i)
		responsesOfCodes[i] = 0;

	int answer_index = 0;
	qreal min_code_weight = 1000000000;
	qreal code_weight;

    for (int code_index = 0; code_index < mSmallPossibles.size; ++code_index) {
        if(mInterupt)
			return;

		int whites, blacks;
        foreach(int possible_index, mPossibles) {
            COMPARE(mCodes.index[mSmallPossibles.index[code_index]], mCodes.index[possible_index], mColors, mPegs, blacks, whites);
			++responsesOfCodes[(blacks+whites)*(blacks+whites+1)/2 + blacks];
		}
		code_weight = computeWeight(responsesOfCodes);

		if (code_weight < min_code_weight) {
			answer_index = code_index;
			min_code_weight = code_weight;
		}
	}

    if(mGuess->mAlgorithm == Algorithm::MostParts)
        min_code_weight = mMaxResponse - 2 - min_code_weight;

    mGuess->mWeight = qFloor(min_code_weight);

    mGuess->setGuess(mCodes.index[mSmallPossibles.index[answer_index]]);
}

qreal Solver::computeWeight(int *m_responses) const
{
	qreal answer = 0;

    switch (mGuess->mAlgorithm) {
	case Algorithm::ExpectedSize:
        for(int i = 0; i < mMaxResponse-2; ++i) {
			answer += ipow(m_responses[i], 2);
			m_responses[i] = 0;
		}
		break;
	case Algorithm::WorstCase:
        for(int i = 0; i < mMaxResponse-2; ++i) {
			answer = qMax((qreal)m_responses[i], answer);
			m_responses[i] = 0;
		}
		break;
	default:	//	Most Parts
        for(int i = 0; i < mMaxResponse-2; ++i) {
			if (m_responses[i] == 0)
				++answer;
			else
				m_responses[i] = 0;
		}
		break;
	}

	// This little trick will prefer possibles in tie
    if (m_responses[mMaxResponse - 1] != 0) {
		answer -= 0.5;
        if (mGuess->mAlgorithm == Algorithm::MostParts)
			--answer;
		else
			++answer;

        m_responses[mMaxResponse - 1] = 0;
	}

	return answer;
}

void Solver::nextCodeSameColor(unsigned char *X)
{
    int i = mPegs - 1;
    int N = mColors;
	N--;
	while (i >= 0 && X[i] >= N)
		X[i--] = 0;
	if (i < 0) return;
	X[i]++;
}

void Solver::nextCodeDifferentColor(unsigned char *X)
{
    int i = mPegs;
	while (--i >= 0) {
        while (++X[i] < mColors) {
			int j = 0;
			while (j < i && X[j] != X[i])
				++j;
			if (j == i) goto mark;
		}
	}
	return;
mark:
    while (++i < mPegs) {
		X[i] = -1;
        while (++X[i] < mColors) {
			int j = 0;
			while (j < i && X[j] != X[i])
				++j;
			if (j == i) break;
		}
	}
}

QString Solver::arrayToString(const unsigned char *m_array) const
{
	QString answer = "";
    for(int i = 0; i < mPegs; ++i) {
		answer.append(QString::number(m_array[i]));
	}
	return answer;
}
