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
#include <QtCore/qmath.h>
#include <QElapsedTimer>

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

Game::Game(const int &peg_no, const int &color_no, const bool &allow_same_color, QObject *parent):
	QThread(parent),
	mPegNumber(peg_no),
	mColorNumber(color_no),
	mAllowSameColor(allow_same_color),
	mLastMinWeight(-1),
	mInterupt(true),
	mAllCodes(0),
	mFirstPossibleCodes(0)
{
	createTables();
	onReset(peg_no, color_no, mAllowSameColor);
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

	mAllCodes = new int *[mAllCodesSize];

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

void Game::onReset(const int &peg_no, const int &color_no, const bool &allow_same_color)
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

	mLastMinWeight = -1;
	mGuess = "";
}
//-----------------------------------------------------------------------------

void Game::onStartGuessing(const Algorithm &alg)
{
	mInterupt = false;
	mAlgorithm = alg;
	start(QThread::NormalPriority);
}
//-----------------------------------------------------------------------------

QString Game::randomPermutation(QString str) const
{
	QString answer = "";
	int n = str.length();
	for(int i = 0; i < n; ++i)
	{
		int j = qrand() % (n - i);
		answer.append(str.at(j));
		str.remove(j, 1);
	}
	return answer;
}
//-----------------------------------------------------------------------------

void Game::permute(QString &code) const
{
	QString permutatedColors = randomPermutation(QString("0123456789").left(mColorNumber));
	code = randomPermutation(code);

	for(int i = 0; i < code.length(); ++i)
	{
		code.replace(i, 1, permutatedColors.at(code.at(i).digitValue()));
	}
}
//-----------------------------------------------------------------------------

int Game::compare(const int *codeA, const int *codeB) const
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
	for(int i = 0; i< mColorNumber; ++i)
	{
		c[i] = 0;
		g[i] = 0;
	}

	int blacks = 0;
	int whites = 0;

	for (int i = 0; i < mPegNumber; ++i)
	{
		if (codeA[i] == codeB[i])
		{
			++blacks;
			continue;
		}

		if (c[codeB[i]] != 0)
		{
			--c[codeB[i]];
			++whites;
		}
		else
		{
			++g[codeB[i]];
		}

		if (g[codeA[i]] != 0)
		{
			--g[codeA[i]];
			++whites;
		}
		else
		{
			++c[codeA[i]];
		}
	}

	int total = blacks + whites;		//	blacks + whites, since we don't need whites below

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
		if (mAllCodesSize <= 10000)
		{
			mFirstPossibleCodesSize = mAllCodesSize;
			mFirstPossibleCodes = new int[mFirstPossibleCodesSize];
			for(int i = 0; i < mFirstPossibleCodesSize; ++i)
				mFirstPossibleCodes[i] = i;
		}
		else if (mPossibleCodes.size() <= 10000)
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

void Game::run()
{
	makeGuess();
	emit finished();
	if (!mInterupt)
		emit guessDoneSignal(mAlgorithm, mGuess, mPossibleCodes.size(), mLastMinWeight);
}
//-----------------------------------------------------------------------------

QString Game::getFirstGuess() const
{
	QString answer;
	QString str;
	if (mAllowSameColor)
	{
		switch (mColorNumber) {
		case 2:
			str = (QString ) "010101";
			answer = str.left(mPegNumber);
			break;
		case 3:
			str = (QString ) "01212";
			answer = str.left(mPegNumber);
			break;
		case 4:
			str = (QString ) "01223";
			answer = str.left(mPegNumber);
			break;
		default:
			str = (QString ) "01223";
			answer = str.left(mPegNumber);
			break;
		}

		if(mColorNumber == 6 && mPegNumber == 4 && mAlgorithm == Algorithm::WorstCase) // the classic game is best with this first guess
			answer = (QString ) "0011";
	}
	else
	{
		QString str = "01234";
		answer = str.left(mPegNumber);
	}

	permute(answer);
	return answer;
}
//-----------------------------------------------------------------------------

void Game::makeGuess()
{
	/*	This function create a guess based on the algorithm. The first guess
	 *	and guess when there are at least 10000 codes possibles are treated
	 *	differently.
	 */
	if (mGuess == "")	// The first guess here
	{
		mGuess = getFirstGuess();
		return;
	}

	if (mPossibleCodes.size() == 1)
	{
		mGuess =  arrayToString(mAllCodes[mPossibleCodes.first()]);
		return;
	}

	if(mPossibleCodes.size() > 10000)
	{
		mGuess = arrayToString(mAllCodes[mPossibleCodes.at(mPossibleCodes.size() >> 1)]);
		return;
	}

	int responsesOfCodes[mResponseSpaceSize];
	for(int i = 0; i < mResponseSpaceSize; ++i)
		responsesOfCodes[i] = 0;

	int answerIndex = 0;
	qreal minWeight = 1000000000;
	qreal weight;

	for (int codeIndex = 0; codeIndex < mFirstPossibleCodesSize; ++codeIndex)
	{
		if(mInterupt)
			return;

		foreach (int possibleIndex, mPossibleCodes)
		{
			++responsesOfCodes[compare(mAllCodes[mFirstPossibleCodes[codeIndex]], mAllCodes[possibleIndex])];
		}
		weight = computeWeight(responsesOfCodes);

		if (weight < minWeight)
		{
			answerIndex = codeIndex;
			minWeight = weight;
		}
	}

	if(mAlgorithm == Algorithm::MostParts)
		minWeight = mResponseSpaceSize - 2 - minWeight;

	mLastMinWeight = qFloor(minWeight);

	mGuess = arrayToString(mAllCodes[mFirstPossibleCodes[answerIndex]]);
}
//-----------------------------------------------------------------------------

qreal Game::computeWeight(int *responses) const
{
	qreal answer = 0;

	switch (mAlgorithm) {
	case Algorithm::ExpectedSize:
		for(int i = 0; i < mResponseSpaceSize-2; ++i)
		{
			answer += ipow(responses[i], 2);
			responses[i] = 0;
		}
		break;
	case Algorithm::WorstCase:
		for(int i = 0; i < mResponseSpaceSize-2; ++i)
		{
			answer = qMax((qreal)responses[i], answer);
			responses[i] = 0;
		}
		break;
	default:	//	Most Parts
		for(int i = 0; i < mResponseSpaceSize-2; ++i)
		{
			if (responses[i] == 0)
				++answer;
			else
				responses[i] = 0;
		}
		break;
	}

	// This little trick will prefer possibles
	if (responses[mResponseSpaceSize - 1] != 0)
	{
		answer -= 0.5;
		if (mAlgorithm == Algorithm::MostParts)
			--answer;
		else
			++answer;

		responses[mResponseSpaceSize - 1] = 0;
	}

	return answer;
}
//-----------------------------------------------------------------------------

void Game::convertBase(int decimal, const int &base, const int &precision, int *convertedArray)
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


