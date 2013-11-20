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

#include "game.h"

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

Game::Game(const int &peg_no, const int &color_no, const bool &allow_same_color):
	mPegNumber(peg_no),
	mColorNumber(color_no),
	mAllowSameColor(allow_same_color),
	mFirstPossibleCodes(0),
	mInformation("")
{
	createTables();
	reset(peg_no, color_no, mAllowSameColor);
}
//-----------------------------------------------------------------------------

Game::~Game()
{
	deleteTables();
}
//-----------------------------------------------------------------------------

void Game::createTables()
{
	if (mAllowSameColor){
		mAllCodesSize = ipow(mColorNumber, mPegNumber);
	} else {
		mAllCodesSize = 1;
		for(int i = 0; i < mPegNumber; ++i)
			mAllCodesSize *= (mColorNumber - i);
	}

	mResponseSpaceSize = (mPegNumber + 1)*(mPegNumber + 2)/2;

	mAllCodes = new int* [mAllCodesSize];

	for(int i = 0; i < mAllCodesSize; ++i)
	{
		mAllCodes[i] = new int[mPegNumber];
		convertBase(i, mColorNumber, mPegNumber, mAllCodes[i]);
	}
}
//-----------------------------------------------------------------------------

void Game::deleteTables()
{
	for(int i = 0; i < mAllCodesSize; ++i)
		delete[] mAllCodes[i];

	delete[] mAllCodes;
	mAllCodes = NULL;

	if(mFirstPossibleCodes)
	{
		delete[] mFirstPossibleCodes;
		mFirstPossibleCodes = NULL;
	}
}
//-----------------------------------------------------------------------------

void Game::reset(const int &peg_no, const int &color_no, const bool &allow_same_color)
{
	if(mColorNumber != color_no || mPegNumber != peg_no || mAllowSameColor != allow_same_color)
	{
		deleteTables();
		mColorNumber = color_no;
		mPegNumber = peg_no;
		mAllowSameColor = allow_same_color;
		createTables();
	}

	if(mFirstPossibleCodes)
	{
		delete[] mFirstPossibleCodes;
		mFirstPossibleCodes = NULL;
	}

	mPossibleCodes.clear();
	for(int i = 0; i < mAllCodesSize; ++i)
		mPossibleCodes.append(i);//in case of not same color here must change

	mInformation = "";
}
//-----------------------------------------------------------------------------

int Game::compare(const int* codeA, const int* codeB) const
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

	for (int i = 0; i < mPegNumber; ++i) {
		blacks += (codeA[i] == codeB[i]);
		++c[codeA[i]];
		++g[codeB[i]];
	}

	int total = 0;		//	blacks + whites, since we don't need whites below

	for (int i = 0; i < mColorNumber; ++i)
	{
		total += qMin(c[i],g[i]);
	}

	return total*(total + 1)/2 + blacks;
}
//-----------------------------------------------------------------------------

bool Game::setResponse(const int &response)
{
	/*	This function is responsible for puting the response and erase the impossibles.
	 *	It will return false if the response is not possible and do nothing
	 */
	QList<int> tempPossibleCodes;
	int mGuessArray[mPegNumber];
	stringToArray(mGuess, mGuessArray);

	foreach (int possible, mPossibleCodes)
	{
		if(compare(mGuessArray, mAllCodes[possible]) == response)
			tempPossibleCodes.append(possible);
	}

	if (tempPossibleCodes.isEmpty())
		return false;

	mPossibleCodes = tempPossibleCodes;
	mResponse = response;

	/*	The first time algorithm comes here, it decide to fill in the mFirstPossibleCodes.
	 *	If mAllCodesSize > 10000, we stick to the possible codes, as it is time consuming.
	 */
	if (!mFirstPossibleCodes)
	{
		if (mAllCodesSize < 10000)
		{
			mFirstPossibleCodesSize = mAllCodesSize;
			mFirstPossibleCodes = new int[mFirstPossibleCodesSize];
			for(int i = 0; i < mFirstPossibleCodesSize; ++i)
				mFirstPossibleCodes[i] = i;
		}
		else if (mPossibleCodes.size() < 10000)
		{
			mFirstPossibleCodesSize = mPossibleCodes.size();
			mFirstPossibleCodes = new int[mFirstPossibleCodesSize];
			for(int i = 0; i < mFirstPossibleCodesSize; ++i)
				mFirstPossibleCodes[i] = mPossibleCodes.at(i);
		}
	}

	return true;
}
//-----------------------------------------------------------------------------

QString Game::getGuess()
{
	//NOT COMPLETED, BEWARE THAT YOU HAVE TO REVISE THIS.

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
	/*	This function create a guess based on the algorithm. The first guess
	 *	and guess when there are at least 10000 codes possibles are treated
	 *	differently.
	 */
	if (mPossibleCodes.size() == 1)
	{
		mGuess =  arrayToString(mAllCodes[mPossibleCodes.first()]);
		mInformation = "The Code Is Cracked!";
		return mGuess;
	}

	if(mPossibleCodes.size() >= 10000)
	{
		mGuess = arrayToString(mAllCodes[mPossibleCodes.at(mPossibleCodes.size() >> 1)]);
		QString str;
		str.setNum(mPossibleCodes.size());
		mInformation = QString("Random Guess    Remaining: %1").arg(str);
		return mGuess;
	}

	int responsesOfCodes[mResponseSpaceSize];
	std::fill(responsesOfCodes, responsesOfCodes + mResponseSpaceSize, 0);

	int answerIndex;
	int minWeight = 1000000000;
	bool isInPossibles(false);

	for (int codeIndex = 0; codeIndex < mFirstPossibleCodesSize; ++codeIndex)
	{

		foreach (int possibleIndex, mPossibleCodes)
			++responsesOfCodes[compare(mAllCodes[mFirstPossibleCodes[codeIndex]], mAllCodes[possibleIndex])];

		int Weight = computeWeight(responsesOfCodes, alg);

		if (Weight < minWeight || (Weight == minWeight && !isInPossibles))
		{
			answerIndex = codeIndex;
			minWeight = Weight;
			isInPossibles = mPossibleCodes.contains(codeIndex);
		}
	}


	if(alg == Algorithm::MostParts)
		minWeight = mResponseSpaceSize - minWeight;

	QString str[3];
	switch (alg) {
	case Algorithm::ExpectedSize:
		str[0] = "Expected Size";
		break;
	case Algorithm::WorstCase:
		str[0] = "Worst Case";
		break;
	default:	//MostParts
		str[0] = "Most Parts";
		break;
	}

	str[1].setNum(minWeight);
	str[2].setNum(mPossibleCodes.size());
	mInformation = QString("%1: %2    Remaining: %3").arg(str[0]).arg(str[1]).arg(str[2]);

	mGuess = arrayToString(mAllCodes[mFirstPossibleCodes[answerIndex]]);

	return mGuess;
}

//-----------------------------------------------------------------------------

int Game::computeWeight(int *responses, const Algorithm &alg) const
{
	int answer = 0;

	for(int i = 0; i < mResponseSpaceSize; ++i)
	{
		switch (alg) {
		case Algorithm::ExpectedSize:
			answer += ipow(responses[i], 2);
			break;
		case Algorithm::WorstCase:
			answer = qMax(responses[i], answer);
			break;
		default:	//	Most Parts
			answer += (responses[i] == 0);
			break;
		}

		responses[i] = 0;
	}
	return answer;
}
//-----------------------------------------------------------------------------

void Game::convertBase(int decimal, const int& base, const int& precision, int* convertedArray)
{

	if(mAllowSameColor)
	{
		for(int i = 0; i < precision; ++i)
		{
			convertedArray[i] = decimal % base;
			decimal /= base;
		}
	}
	else
	{
		QString NUMS = "0123456789"; // Characters that may be used

		int maxDigits = mAllCodesSize;
		for(int i = 0; i < precision; ++i)
		{
			maxDigits /= base - i;
			int temp = decimal / maxDigits;
			convertedArray[i] = NUMS[temp].digitValue();
			NUMS.remove(temp, 1);
			decimal -= temp*maxDigits;
		}
	}
}

//-----------------------------------------------------------------------------

QString Game::arrayToString(const int *ar) const
{
	QString answer = "";
	QString temp;
	for(int i = 0; i < mPegNumber; ++i)
		answer.append(temp.setNum(ar[i]));
	return answer;
}
//-----------------------------------------------------------------------------

void Game::stringToArray(const QString &str, int *arr) const
{
	for(int i = 0; i < mPegNumber; ++i)
		arr[i] = str[i].digitValue();
}


