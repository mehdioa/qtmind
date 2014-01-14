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
#include "gamerules.h"
#include "guesselement.h"
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

Solver::Solver(GameRules *game_rules, GuessElement *guess_element, QObject *parent):
	QThread(parent),
	gameRules(game_rules),
	guessElement(guess_element),
	interupt(true),
	allCodes(0),
	firstPossibleCodes(0)
{
	guessElement->algorithm = gameRules->algorithm;
	onReset();
}
//-----------------------------------------------------------------------------

Solver::~Solver()
{
	deleteTables();
}
//-----------------------------------------------------------------------------

bool Solver::done() const
{
	return (guessElement->response == responseSpaceSize - 1);
}
//-----------------------------------------------------------------------------

void Solver::createTables()
{
	if (gameRules->sameColorAllowed)
	{
		allCodesSize = ipow(gameRules->colorNumber, gameRules->pegNumber);
	}
	else
	{
		allCodesSize = 1;
		for(int i = 0; i < gameRules->pegNumber; ++i)
			allCodesSize *= (gameRules->colorNumber - i);
	}

	responseSpaceSize = (gameRules->pegNumber + 1)*(gameRules->pegNumber + 2)/2;

	allCodes = new int *[allCodesSize];

	for(int i = 0; i < allCodesSize; ++i)
	{
		allCodes[i] = new int[gameRules->pegNumber];
		indexToCode(i);
	}

	for(int i = 0; i < allCodesSize; ++i)
		possibleCodes.append(i);

}
//-----------------------------------------------------------------------------

void Solver::deleteTables()
{
	if (allCodes)
	{
		for(int i = 0; i < allCodesSize; ++i)
			delete[] allCodes[i];

		delete[] allCodes;
		allCodes = NULL;
	}

	if(firstPossibleCodes)
	{
		delete[] firstPossibleCodes;
		firstPossibleCodes = NULL;
	}

	possibleCodes.clear();
}
//-----------------------------------------------------------------------------

void Solver::onReset()
{
	deleteTables();
	createTables();
	guessElement->reset(gameRules->algorithm, allCodesSize);
}
//-----------------------------------------------------------------------------

void Solver::onStartGuessing()
{
	interupt = false;
	guessElement->algorithm = gameRules->algorithm;
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
	QString shuffled_colors = shuffle(QString("0123456789").left(gameRules->colorNumber));
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
	int c[gameRules->colorNumber];
	int g[gameRules->colorNumber];
	std::fill(c, c + gameRules->colorNumber, 0);
	std::fill(g, g + gameRules->colorNumber, 0);
	int blacks = 0;

	for (int i = 0; i < gameRules->pegNumber; ++i)
	{
		if (codeA[i] == codeB[i])
			++blacks;
		++c[codeA[i]];
		++g[codeB[i]];
	}

	int total = 0;//	blacks + whites, since we don't need whites below
	for(int i = 0; i < gameRules->colorNumber; ++i)
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
	int guessArray[gameRules->pegNumber];
	stringToArray(guessElement->guess, guessArray);

	foreach(int possible, possibleCodes)
	{
		if(compare(guessArray, allCodes[possible]) == m_response)
			tempPossibleCodes.append(possible);
	}

	if (tempPossibleCodes.isEmpty())
		return false;

	possibleCodes = tempPossibleCodes;
	guessElement->response = m_response;
	guessElement->possibles = possibleCodes.size();
	setFirstPossibles();
	return true;
}
//-----------------------------------------------------------------------------

void Solver::setFirstPossibles()
{
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
}
//-----------------------------------------------------------------------------

void Solver::run()
{
	makeGuess();
	if (!interupt)
		emit guessDoneSignal();
}
//-----------------------------------------------------------------------------

QString Solver::getFirstGuess() const
{
	QString answer;
	QString str;
	if (gameRules->sameColorAllowed)
	{
		switch (gameRules->colorNumber) {
		case 2:
			str = (QString) "01010";
			answer = str.left(gameRules->pegNumber);
			break;
		case 3:
			str = (QString) "01212";
			answer = str.left(gameRules->pegNumber);
			break;
		case 4:
			str = (QString) "01223";
			answer = str.left(gameRules->pegNumber);
			break;
		default:
			str = (QString) "01223";
			answer = str.left(gameRules->pegNumber);
			break;
		}

		// the classic game (c = 6, p = 4) is best with this first guess on Worst Case

		if(gameRules->colorNumber == 6 && gameRules->pegNumber == 4 && gameRules->algorithm == Algorithm::WorstCase)
			answer = (QString) "0011";
	}
	else
	{
		QString str = "01234";
		answer = str.left(gameRules->pegNumber);
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
	if (possibleCodes.size() == allCodesSize)	// The first guess here
	{
		guessElement->guess = getFirstGuess();
		return;
	}

	if (possibleCodes.size() == 1)
	{
		guessElement->guess =  arrayToString(allCodes[possibleCodes.first()]);
		return;
	}

	if(possibleCodes.size() > 10000)
	{
		guessElement->guess = arrayToString(allCodes[possibleCodes.at(possibleCodes.size() >> 1)]);
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

		foreach(int possible_index, possibleCodes)
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

	if(guessElement->algorithm == Algorithm::MostParts)
		min_code_weight = responseSpaceSize - 2 - min_code_weight;

	guessElement->weight = qFloor(min_code_weight);

	guessElement->guess = arrayToString(allCodes[firstPossibleCodes[answer_index]]);
}
//-----------------------------------------------------------------------------

qreal Solver::computeWeight(int *m_responses) const
{
	qreal answer = 0;

	switch (guessElement->algorithm) {
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
		if (guessElement->algorithm == Algorithm::MostParts)
			--answer;
		else
			++answer;

		m_responses[responseSpaceSize - 1] = 0;
	}

	return answer;
}
//-----------------------------------------------------------------------------

void Solver::indexToCode(const int &number)
{
	int m_number = number;
	if(gameRules->sameColorAllowed)
	{
		for(int i = 0; i < gameRules->pegNumber; ++i)
		{
			allCodes[number][i] = m_number % gameRules->colorNumber;
			m_number /= gameRules->colorNumber;
		}
	}
	else
	{
		QString NUMS = "0123456789"; // Characters that may be used

		int max_digits = allCodesSize;
		for(int i = 0; i < gameRules->pegNumber; ++i)
		{
			max_digits /= gameRules->colorNumber - i;
			int temp = m_number / max_digits;
			allCodes[number][i] = NUMS[temp].digitValue();
			NUMS.remove(temp, 1);
			m_number -= temp*max_digits;
		}
	}
}
//-----------------------------------------------------------------------------

QString Solver::arrayToString(const int *m_array) const
{
	QString answer = "";
	for(int i = 0; i < gameRules->pegNumber; ++i)
		answer.append(QString::number(m_array[i]));
	return answer;
}
//-----------------------------------------------------------------------------

void Solver::stringToArray(const QString &m_string, int *m_array) const
{
	for(int i = 0; i < gameRules->pegNumber; ++i)
		m_array[i] = m_string[i].digitValue();
}
