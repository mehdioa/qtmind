/***********************************************************************
 *
 * Copyright (C) 2013 Mehdi Omidal <mehdioa@gmail.com>
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

#include "game.h"
#include <cmath>
#include <QDebug>

Game::Game(const int &peg_no, const int &color_no, const bool &allow_same_color):
	mPegNumber(peg_no),
	mColorNumber(color_no),
	mAllowSameColor(allow_same_color),
	mFirstPossibleCodes(0)
{
	setAllCodesSize();
	mResponseSpaceSize = (mPegNumber + 1)*(mPegNumber + 2)/2;
	fillAllCodes();
	reset(peg_no, color_no, mAllowSameColor);
}

Game::~Game()
{
	clearAllCodes();
}
//-----------------------------------------------------------------------------

void Game::reset(const int &peg_no, const int &color_no, const bool &allow_same_color)
{
	if(mColorNumber != color_no || mPegNumber != peg_no || mAllowSameColor != allow_same_color)
	{
		mColorNumber = color_no;
		mPegNumber = peg_no;
		mAllowSameColor = allow_same_color;
		mResponseSpaceSize = (mPegNumber + 1)*(mPegNumber + 2)/2;
		setAllCodesSize();
		clearAllCodes();
		fillAllCodes();
	}

	if(mFirstPossibleCodes)
	{
		delete[] mFirstPossibleCodes;
		mFirstPossibleCodes = NULL;
	}
	mPossibleCodes.clear();
	for(int i = 0; i < mAllCodesSize; ++i)
		mPossibleCodes.append(i);//in case of not same color here must change
}
//-----------------------------------------------------------------------------


int Game::compare(const QString codeA, const QString codeB) const
{
	/*	Compare codeA to codeB and return the black-white code response.
	 *	Interestingly,  total can be computed as
	 *
	 *	$$ total := whites + blacks = ( \sum_{i=1}^6 \min(c_i, g_i)) $$
	 *
	 *	where c_i is the number of times the color i is in the code and
	 *  g_i is the number of times it is in the guess. See
	 *	http://mathworld.wolfram.com/Mastermind.html for more information
	 */
	int c[mColorNumber];
	int g[mColorNumber];
	std::fill(c, c+mColorNumber, 0);
	std::fill(g, g+mColorNumber, 0);

	int blacks = 0;
	QString NUMS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (int i = 0; i < mPegNumber; ++i) {
		blacks += (codeA[i] == codeB[i]);
		++c[NUMS.indexOf(codeA[i])];
		++g[NUMS.indexOf(codeB[i])];
	}

	int total = 0;		//	blacks + whites, since we don't need whites below

	for (int i = 0; i < mColorNumber; ++i)
	{
		total += std::min(c[i],g[i]);
	}

	return total*(total + 1)/2 + blacks;
}
//-----------------------------------------------------------------------------

bool Game::done() const
{
	return (mResponse == mResponseSpaceSize - 1);
}

//-----------------------------------------------------------------------------

bool Game::setResponse(const int &response)
{
	/*	This function is responsible for puting the response and erase the impossibles
	 *	it will return false if the response is not possible and do nothing
	 */
	QList<int> tempPossibleCodes;

	foreach (int possible, mPossibleCodes)
	{
		if(compare(mGuess, mAllCodes[possible]) == response)
			tempPossibleCodes.append(possible);
	}

	if (tempPossibleCodes.isEmpty())
		return false;

	mPossibleCodes = tempPossibleCodes;
	mResponse = response;

	if (!mFirstPossibleCodes)
	{
		if (mAllCodesSize > 10000)
		{
			mFirstPossibleCodes = new int[mPossibleCodes.size()];
			mFirstPossibleCodesSize = mPossibleCodes.size();
			for(int i = 0; i < mPossibleCodes.size(); ++i)
				mFirstPossibleCodes[i] = mPossibleCodes.at(i);
		}
		else
		{
			mFirstPossibleCodes = new int[mAllCodesSize];
			mFirstPossibleCodesSize = mAllCodesSize;
			for(int i = 0; i < mAllCodesSize; ++i)
				mFirstPossibleCodes[i] = i;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------

QString Game::getGuess()
{
	/*	Used for first guess */
	if (mAllowSameColor)
	{
		switch (mPegNumber) {
		case 2:
			mGuess = (QString) "01";
			break;
		case 3:
			mGuess = (QString) "012";
			break;
		case 4:
			mGuess = (QString) "1100";
			break;
		case 5:
			mGuess = (QString) "00123";
			break;
		default:
			QString str = "01010101010101010101010101";
			mGuess = str.left(mPegNumber);
			break;
		}
	}
	else
	{
		QString str = "0123456789";
		mGuess = str.left(mPegNumber);
	}
	return mGuess;
}
//-----------------------------------------------------------------------------

QString Game::getGuess(const Algorithm& alg)
{
	makeGuess(alg);
	return mGuess;
}
//-----------------------------------------------------------------------------

bool Game::hasDifferentSetup(const int &peg_no, const int color_no) const
{
	return (peg_no != mPegNumber || color_no != mColorNumber);
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

void Game::makeGuess(Algorithm alg)
{
	if (mPossibleCodes.size() == 1) {
		mGuess =  mAllCodes[mPossibleCodes.first()];
		return;
	}

	switch (alg) {
	case WorstCase:
		mGuess = worstCaseAlgorithm();
		break;
	case ExpectedSize:
		mGuess = expectedSizeAlgorithm();
		break;
	default://MostParts
		mGuess = mostPartsAlgorithm();
		break;
	}
}


//-----------------------------------------------------------------------------

void Game::fillAllCodes()
{
	mAllCodes = new QString [mAllCodesSize];

	for(int i = 0; i < mAllCodesSize; ++i)
		mAllCodes[i] = convertBase(i, mColorNumber, mPegNumber);
}
//-----------------------------------------------------------------------------

void Game::clearAllCodes()
{
	delete[] mAllCodes;
	mAllCodes = NULL;

	delete[] mFirstPossibleCodes;
	mFirstPossibleCodes = NULL;
}
//-----------------------------------------------------------------------------

QString Game::mostPartsAlgorithm() const
{
//	if(mPossibleCodes.size()>1000){
//		return (mAllCodes[mPossibleCodes.first()]);
//	}
	QVector<int> parts(mFirstPossibleCodesSize, 0);
	QVector<int> responsesOfCodes(mResponseSpaceSize);

	int answerIndex;
	int max = 0;
	bool isInPossibles(false);

	for (int codeIndex = 0; codeIndex < mFirstPossibleCodesSize; ++codeIndex)
	{
		responsesOfCodes.fill(0);

		foreach (int possibleIndex, mPossibleCodes)
			responsesOfCodes.replace(compare(mAllCodes[codeIndex], mAllCodes[possibleIndex]), 1);

		parts.insert(codeIndex, responsesOfCodes.count(1));// numbur of possible responses

		if (parts[codeIndex] > max || (parts[codeIndex] == max && !isInPossibles))
		{
			answerIndex = codeIndex;
			max = parts[codeIndex];
			isInPossibles = mPossibleCodes.contains(codeIndex);
		}
	}

	return mAllCodes[answerIndex];
}
//-----------------------------------------------------------------------------

QString Game::expectedSizeAlgorithm() const
{
	QVector<double> expected(mAllCodesSize, 0);
	QVector<unsigned short> responsesOfCodes(mResponseSpaceSize);

	for (int codeIndex = 0; codeIndex < mAllCodesSize; ++codeIndex) {
		responsesOfCodes.fill(0);

		foreach (int possible, mPossibleCodes) {
			int index = compare(mAllCodes[codeIndex], mAllCodes[possible]);
			responsesOfCodes.replace(index, responsesOfCodes.at(index)+1);
		}
		for (int j = 0; j < mResponseSpaceSize; ++j) {
			double value = expected.at(codeIndex);
			expected.replace(codeIndex, value + std::pow((double)responsesOfCodes.at(j), 2.295));
		}
	}
	int answerIndex = mPossibleCodes.first();
	double minExpected = expected.at(answerIndex);
	double testForMinExpected;
	foreach (int index, mPossibleCodes) {//First we find the most parts between possibles
		testForMinExpected = expected.at(index);
		if (testForMinExpected < minExpected) {
			answerIndex = index;
			minExpected = testForMinExpected;
		}
	}
	for(int index = 0; index < mAllCodesSize; ++index) {//Now we check whole set
		testForMinExpected = expected.at(index);
		if (testForMinExpected < minExpected) {
			answerIndex = index;
			minExpected = testForMinExpected;
		}
	}
	return mAllCodes[answerIndex];
}
//-----------------------------------------------------------------------------

QString Game::worstCaseAlgorithm() const
{
	QVector<int> worsts(mAllCodesSize, 0);
	QVector<int> responsesOfCodes(mResponseSpaceSize);

	for (int codeIndex = 0; codeIndex < mAllCodesSize; ++codeIndex) {
		responsesOfCodes.fill(0);

		foreach (int possible, mPossibleCodes) {
			int index = compare(mAllCodes[codeIndex], mAllCodes[possible]);
			responsesOfCodes.replace(index, responsesOfCodes.at(index)+1);
		}

		int max = 0;
		foreach (int response, responsesOfCodes) {
			max = (response > max ? response : max);
		}

		worsts.replace(codeIndex, max);// numbur of possible responses
	}

	int answerIndex = mPossibleCodes.first();
	int worstCase = worsts.at(answerIndex);
	int testForWorstCase;

	foreach (int index, mPossibleCodes) {//First we find the most parts between possibles
		testForWorstCase = worsts.at(index);
		if (testForWorstCase < worstCase) {
			answerIndex = index;
			worstCase = testForWorstCase;
		}
	}

	for (int index = 0; index < mAllCodesSize; ++index) {//First we find the most parts between possibles
		testForWorstCase = worsts.at(index);
		if (testForWorstCase < worstCase) {
			answerIndex = index;
			worstCase = testForWorstCase;
		}
	}

	return mAllCodes[answerIndex];
}
//-----------------------------------------------------------------------------

QString Game::convertBase(int decimal, const int& base, const int& precision)
{
	QString result;
	if(mAllowSameColor){
		result.setNum(decimal, base);
		result = result.rightJustified(precision, '0');
		return result;
	} else {
		QString NUMS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // Characters that may be used
		int maxDigits = mAllCodesSize;
		for(int i = 0; i < mPegNumber; ++i){
			maxDigits /= mColorNumber - i;
			int temp = decimal / maxDigits;
			result[i] = NUMS[temp];
			NUMS.remove(temp, 1);
			decimal -= temp*maxDigits;
		}
		return result;
	}
}
//-----------------------------------------------------------------------------

inline int ipow(int base, int exp)
{
	int result = 1;
	while (exp) {
		if (exp & 1)
			result *= base;
		exp >>= 1;
		base *= base;
	}

	return result;
}
//-----------------------------------------------------------------------------

void Game::setAllCodesSize()
{
	if (mAllowSameColor){
		mAllCodesSize = ipow(mColorNumber, mPegNumber);
	} else {
		mAllCodesSize = 1;
		for(int i = 0; i < mPegNumber; ++i)
			mAllCodesSize *= (mColorNumber - i);
	}
}
