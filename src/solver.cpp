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
#include "ctime"
#include <QtCore/qmath.h>

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

Solver::Solver(const int &peg_no, const int &color_no, const bool &allow_same_color, QObject *parent):
	QThread(parent),
	pegNumber(peg_no),
	colorNumber(color_no),
	allowSameColor(allow_same_color),
	lastMinWeight(-1),
	interupt(true),
	allCodes(0),
	firstPossibleCodes(0)
{
	createTables();
	onReset(peg_no, color_no, allowSameColor);
}
//-----------------------------------------------------------------------------

Solver::~Solver()
{
	deleteTables();
}
//-----------------------------------------------------------------------------

void Solver::createTables()
{
	if (allowSameColor){
		allCodesSize = ipow(colorNumber, pegNumber);
	} else {
		allCodesSize = 1;
		for(int i = 0; i < pegNumber; ++i)
			allCodesSize *= (colorNumber - i);
	}

	responseSpaceSize = (pegNumber + 1)*(pegNumber + 2)/2;

	allCodes = new int *[allCodesSize];

	for(int i = 0; i < allCodesSize; ++i)
	{
		allCodes[i] = new int[pegNumber];
		convertBase(i, colorNumber, pegNumber, allCodes[i]);
	}
}
//-----------------------------------------------------------------------------

void Solver::deleteTables()
{
	for(int i = 0; i < allCodesSize; ++i)
		delete[] allCodes[i];

	delete[] allCodes;
	allCodes = NULL;

	if(firstPossibleCodes)
	{
		delete[] firstPossibleCodes;
		firstPossibleCodes = NULL;
	}
}
//-----------------------------------------------------------------------------

void Solver::onReset(const int &peg_no, const int &color_no, const bool &allow_same_color)
{
	if(colorNumber != color_no || pegNumber != peg_no || allowSameColor != allow_same_color)
	{
		deleteTables();
		colorNumber = color_no;
		pegNumber = peg_no;
		allowSameColor = allow_same_color;
		createTables();
	}

	if(firstPossibleCodes)
	{
		delete[] firstPossibleCodes;
		firstPossibleCodes = NULL;
	}

	possibleCodes.clear();
	for(int i = 0; i < allCodesSize; ++i)
		possibleCodes.append(i);

	lastMinWeight = -1;
	guess = "";
}
//-----------------------------------------------------------------------------

void Solver::onStartGuessing(const Algorithm &m_algorithm)
{
	interupt = false;
	algorithm = m_algorithm;
	start(QThread::NormalPriority);
}
//-----------------------------------------------------------------------------

QString Solver::shuffle(QString m_string) const
{
	/*	This function shuffle the characters in a string */
	qsrand(time(NULL));
	QString answer = "";

	while(m_string.length() > 0)
	{
		int j = qrand() % m_string.length();
		answer.append(m_string.at(j));
		m_string.remove(j, 1);
	}
	return answer;
}
//-----------------------------------------------------------------------------

void Solver::permute(QString &m_code) const
{
	/*	This funstion shuffle the code and do a permutation on the characters */
	QString shuffled_colors = shuffle(QString("0123456789").left(colorNumber));
	m_code = shuffle(m_code);

	for(int i = 0; i < m_code.length(); ++i)
		m_code.replace(i, 1, shuffled_colors.at(m_code.at(i).digitValue()));
}
//-----------------------------------------------------------------------------

int Solver::compare(const int *codeA, const int *codeB) const
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
	int c[colorNumber];
	int g[colorNumber];
	std::fill(c, c+colorNumber, 0);
	std::fill(g, g+colorNumber, 0);
	int blacks = 0;

	for (int i = 0; i < pegNumber; ++i)
	{
		if (codeA[i] == codeB[i])
			++blacks;
		++c[codeA[i]];
		++g[codeB[i]];
	}

	int total = 0;//	blacks + whites, since we don't need whites below
	for(int i = 0; i < colorNumber; ++i)
		total += (c[i] < g[i]) ? c[i] : g[i];

	return total*(total + 1)/2 + blacks;
}
//-----------------------------------------------------------------------------

bool Solver::setResponse(const int &m_response)
{
	/*	This function is responsible for puting the response and erase the impossibles.
	 *	It will return false if the response is not possible and do nothing
	 */
	QList<int> tempPossibleCodes;
	int guessArray[pegNumber];
	stringToArray(guess, guessArray);

	foreach (int possible, possibleCodes)
	{
		if(compare(guessArray, allCodes[possible]) == m_response)
			tempPossibleCodes.append(possible);
	}

	if (tempPossibleCodes.isEmpty())
		return false;

	possibleCodes = tempPossibleCodes;
	response = m_response;

	/*	The first time algorithm comes here, it decide to fill in the firstPossibleCodes.
	 *	If allCodesSize > 10000, we stick to the possible codes, as it is time consuming.
	 *	If neither of allCodesSize or possibleCodes.size() is less than 10001, it will
	 *	postpone filling firstPossibleCodes to next time.
	 */
	if (!firstPossibleCodes)
	{
		if (allCodesSize <= 10000)
		{
			firstPossibleCodesSize = allCodesSize;
			firstPossibleCodes = new int[firstPossibleCodesSize];
			for(int i = 0; i < firstPossibleCodesSize; ++i)
				firstPossibleCodes[i] = i;
		}
		else if (possibleCodes.size() <= 10000)
		{
			firstPossibleCodesSize = possibleCodes.size();
			firstPossibleCodes = new int[firstPossibleCodesSize];
			for(int i = 0; i < firstPossibleCodesSize; ++i)
				firstPossibleCodes[i] = possibleCodes.at(i);
		}
	}
	return true;
}
//-----------------------------------------------------------------------------

void Solver::run()
{
	makeGuess();
	if (!interupt)
		emit guessDoneSignal(algorithm, guess, possibleCodes.size(), lastMinWeight);
}
//-----------------------------------------------------------------------------

QString Solver::getFirstGuess() const
{
	QString answer;
	QString str;
	if (allowSameColor)
	{
		switch (colorNumber) {
		case 2:
			str = (QString) "010101";
			answer = str.left(pegNumber);
			break;
		case 3:
			str = (QString) "01212";
			answer = str.left(pegNumber);
			break;
		case 4:
			str = (QString) "01223";
			answer = str.left(pegNumber);
			break;
		default:
			str = (QString) "01223";
			answer = str.left(pegNumber);
			break;
		}

		// the classic game (c = 6, p = 4) is best with this first guess on Worst Case
		if(colorNumber == 6 && pegNumber == 4 && algorithm == Algorithm::WorstCase)
			answer = (QString) "0011";
	}
	else
	{
		QString str = "01234";
		answer = str.left(pegNumber);
	}

	permute(answer);
	return answer;
}
//-----------------------------------------------------------------------------

void Solver::makeGuess()
{
	/*	This function creates a guess based on the algorithm. The first guess
	 *	and guess when there are at least 10000 codes possibles are treated
	 *	differently.
	 */
	if (guess == "")	// The first guess here
	{
		guess = getFirstGuess();
		return;
	}

	if (possibleCodes.size() == 1)
	{
		guess =  arrayToString(allCodes[possibleCodes.first()]);
		return;
	}

	if(possibleCodes.size() > 10000)
	{
		guess = arrayToString(allCodes[possibleCodes.at(possibleCodes.size() >> 1)]);
		return;
	}

	int responsesOfCodes[responseSpaceSize];
	for(int i = 0; i < responseSpaceSize; ++i)
		responsesOfCodes[i] = 0;

	int answer_index = 0;
	qreal min_code_weight = 1000000000;
	qreal code_weight;

	for (int code_index = 0; code_index < firstPossibleCodesSize; ++code_index)
	{
		if(interupt)
			return;

		foreach (int possible_index, possibleCodes)
		{
			++responsesOfCodes[compare(allCodes[firstPossibleCodes[code_index]], allCodes[possible_index])];
		}
		code_weight = computeWeight(responsesOfCodes);

		if (code_weight < min_code_weight)
		{
			answer_index = code_index;
			min_code_weight = code_weight;
		}
	}

	if(algorithm == Algorithm::MostParts)
		min_code_weight = responseSpaceSize - 2 - min_code_weight;

	lastMinWeight = qFloor(min_code_weight);

	guess = arrayToString(allCodes[firstPossibleCodes[answer_index]]);
}
//-----------------------------------------------------------------------------

qreal Solver::computeWeight(int *m_responses) const
{
	qreal answer = 0;

	switch (algorithm) {
	case Algorithm::ExpectedSize:
		for(int i = 0; i < responseSpaceSize-2; ++i)
		{
			answer += ipow(m_responses[i], 2);
			m_responses[i] = 0;
		}
		break;
	case Algorithm::WorstCase:
		for(int i = 0; i < responseSpaceSize-2; ++i)
		{
			answer = qMax((qreal)m_responses[i], answer);
			m_responses[i] = 0;
		}
		break;
	default:	//	Most Parts
		for(int i = 0; i < responseSpaceSize-2; ++i)
		{
			if (m_responses[i] == 0)
				++answer;
			else
				m_responses[i] = 0;
		}
		break;
	}

	// This little trick will prefer possibles in tie
	if (m_responses[responseSpaceSize - 1] != 0)
	{
		answer -= 0.5;
		if (algorithm == Algorithm::MostParts)
			--answer;
		else
			++answer;

		m_responses[responseSpaceSize - 1] = 0;
	}

	return answer;
}
//-----------------------------------------------------------------------------

void Solver::convertBase(int m_decimal, const int &m_base, const int &m_precision, int *m_convertedArray)
{

	if(allowSameColor)
	{
		for(int i = 0; i < m_precision; ++i)
		{
			m_convertedArray[i] = m_decimal % m_base;
			m_decimal /= m_base;
		}
	}
	else
	{
		QString NUMS = "0123456789"; // Characters that may be used

		int max_digits = allCodesSize;
		for(int i = 0; i < m_precision; ++i)
		{
			max_digits /= m_base - i;
			int temp = m_decimal / max_digits;
			m_convertedArray[i] = NUMS[temp].digitValue();
			NUMS.remove(temp, 1);
			m_decimal -= temp*max_digits;
		}
	}
}
//-----------------------------------------------------------------------------

QString Solver::arrayToString(const int *ar) const
{
	QString answer = "";
	for(int i = 0; i < pegNumber; ++i)
		answer.append(QString::number(ar[i]));
	return answer;
}
//-----------------------------------------------------------------------------

void Solver::stringToArray(const QString &m_string, int *m_array) const
{
	for(int i = 0; i < pegNumber; ++i)
		m_array[i] = m_string[i].digitValue();
}
