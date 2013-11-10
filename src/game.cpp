#include "game.h"
#include <cmath>
#include <QDebug>

Game::Game(const int &peg_no, const int &color_no, const bool &allow_same_color):
	mPegNumber(peg_no),
	mColorNumber(color_no),
	mAllowSameColor(allow_same_color)
{
	setAllCodesSize();
	mResponeSpaceSize = (mPegNumber + 1)*(mPegNumber + 2)/2;
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
	if(mColorNumber != color_no || mPegNumber != peg_no || mAllowSameColor != allow_same_color){
		mColorNumber = color_no;
		mPegNumber = peg_no;
		mAllowSameColor = allow_same_color;
		mResponeSpaceSize = (mPegNumber + 1)*(mPegNumber + 2)/2;
		setAllCodesSize();
		clearAllCodes();
		fillAllCodes();
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

	for (int i = 0; i < mColorNumber; ++i){
		total += std::min(c[i],g[i]);
	}

	return total*(total + 1)/2 + blacks;
}
//-----------------------------------------------------------------------------

bool Game::done() const {
	return (mResponse == mResponeSpaceSize - 1);
}

//-----------------------------------------------------------------------------

bool Game::setResponse(const int &response)
{
	QVector<int> tempPossibleCodes;
	foreach (int possible, mPossibleCodes) {
		if(compare(mGuess, mAllCodes[possible]) == response)
			tempPossibleCodes.append(possible);
	}
	if (tempPossibleCodes.isEmpty()) {
		return false;
	}
	mPossibleCodes = tempPossibleCodes;
	mResponse = response;
	return true;
}
//-----------------------------------------------------------------------------

QString Game::getGuess()
{
	/*	Used for first guess */
	if (mAllowSameColor) {
		switch (mPegNumber) {
		case 2:
			mGuess = (QString) "01";
			break;
		case 3:
			mGuess = (QString) "012";
			break;
		case 4:
			mGuess = (QString) "0011";
			break;
		case 5:
			mGuess = (QString) "00123";
			break;
		default:
			QString str = "01010101010101010101010101";
			mGuess = str.left(mPegNumber);
			break;
		}
	} else {
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
	if (mPossibleCodes.size() == 1)
		mGuess =  mAllCodes[mPossibleCodes.first()];

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

//	for (int i = 0; i < mAllCodesSize; ++i)
//	{
//		mAllCodes[i] = new char [mPegNumber];
//	}
	for(int i = 0; i < mAllCodesSize; ++i)
		mAllCodes[i] = convertBase(i, mColorNumber, mPegNumber);
}
//-----------------------------------------------------------------------------

void Game::clearAllCodes()
{
//	if(mAllCodes) {
//		for (int i = 0; i < mAllCodesSize; ++i)
//		{
//			delete [] mAllCodes[i] ;
//		}
		delete [] mAllCodes;
		mAllCodes = NULL;
//	}
}
//-----------------------------------------------------------------------------

QString Game::mostPartsAlgorithm() const
{
	QVector<int> parts(mAllCodesSize, 0);
	QVector<int> responsesOfCodes(mResponeSpaceSize);

	for (int codeIndex = 0; codeIndex < mAllCodesSize; ++codeIndex) {
		responsesOfCodes.fill(0);
		foreach (int possible, mPossibleCodes) {

			responsesOfCodes.replace(compare(mAllCodes[codeIndex], mAllCodes[possible]), 1);
		}
		parts.insert(codeIndex, responsesOfCodes.count(1));// numbur of possible responses
	}

	int answerIndex = mPossibleCodes.first();
	int maxParts = parts.at(answerIndex);
	int testForMaxParts;

	foreach (int index, mPossibleCodes) {//First we find the most parts between possibles
		testForMaxParts = parts.at(index);
		if (testForMaxParts > maxParts) {
			answerIndex = index;
			maxParts = testForMaxParts;
		}
	}

	for(int index = 0; index < mAllCodesSize; ++index) {//Now we find the most parts between all possibles
		testForMaxParts = parts.at(index);
		if (testForMaxParts > maxParts) {
			answerIndex = index;
			maxParts = testForMaxParts;
		}
	}
	return mAllCodes[answerIndex];
}
//-----------------------------------------------------------------------------

QString Game::expectedSizeAlgorithm() const
{
	QVector<double> expected(mAllCodesSize, 0);
	QVector<unsigned short> responsesOfCodes(mResponeSpaceSize);

	for (int codeIndex = 0; codeIndex < mAllCodesSize; ++codeIndex) {
		responsesOfCodes.fill(0);

		foreach (int possible, mPossibleCodes) {
			int index = compare(mAllCodes[codeIndex], mAllCodes[possible]);
			responsesOfCodes.replace(index, responsesOfCodes.at(index)+1);
		}
		for (int j = 0; j < mResponeSpaceSize; ++j) {
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
	QVector<int> responsesOfCodes(mResponeSpaceSize);

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
		return result.setNum(decimal, base).leftJustified(precision, '0');
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
