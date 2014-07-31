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
#include "rules.h"
#include "guess.h"
#include "ctime"
#include <QtCore/qmath.h>
#include <stdlib.h>

/**
  * @brief compares two codes A and B, which are two int arrays
  * C and P are the number of colors and pegs
  * R is the result
  *
  * Interestingly, total can be computed as
  * f[total := whites + blacks = ( \sum_{i=1}^6 \min(c_i, g_i)) \f]
  * where c_i is the number of times the color i is in the code and g_i is the number of times it is in the guess. See
  * http://mathworld.wolfram.com/Mastermind.html
  * for more information
  */
#define compare(A, B, C, P, R) {\
	int __c[C], __g[C];\
	std::fill(__c, __c+C, 0);\
	std::fill(__g, __g+C, 0);\
	int __blacks = 0;\
	for (int i = 0; i < P; ++i) {\
		if (A[i] == B[i])\
			++__blacks;\
		++__c[A[i]];\
		++__g[B[i]];\
	}\
	int __total = 0;\
	for(int i = 0; i < C; ++i)\
		__total += (__c[i] < __g[i]) ? __c[i] : __g[i];\
	R = __total*(__total + 1)/2 + __blacks;\
}

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

Solver::Solver(Rules *game_rules, Guess *guess_element, QObject *parent):
	QThread(parent),
	rules(game_rules),
	guess(guess_element),
	interupt(true)
{
	codes.index = NULL;
	sp.index = NULL;

	guess->algorithm = rules->algorithm;
	onReset();
}

Solver::~Solver()
{
	deleteTables();
}

bool Solver::done() const
{
	return (guess->response == max_response - 1);
}

void Solver::createTables()
{
	if (rules->same_colors) {
		codes.size = ipow(rules->colors, rules->pegs);
	} else {
		codes.size = 1;
		for(int i = 0; i < rules->pegs; ++i)
			codes.size *= (rules->colors - i);
	}

	max_response = (rules->pegs + 1)*(rules->pegs + 2)/2;

	codes.index = new int *[codes.size];

	for(int i = 0; i < codes.size; ++i) {
		codes.index[i] = new int[rules->pegs];
	}

	if (rules->same_colors) {
		for (int i = 0; i < codes.size; i++)
			indexToCodeSameColor(i);
	} else {
		for (int i = 0; i < codes.size; i++)
			indexToCodeDifferentColor(i);
	}

	for(int i = 0; i < codes.size; ++i)
		possibles.append(i);

}

void Solver::deleteTables()
{
	if (codes.index != NULL) {
		for(int i = 0; i < codes.size; ++i)
			delete[] codes.index[i];

		delete[] codes.index;
		codes.index = NULL;
	}

	if(sp.index) {
		delete[] sp.index;
		sp.index = NULL;
	}

	possibles.clear();
}

void Solver::onReset()
{
	deleteTables();
	createTables();
	guess->reset(rules->algorithm, codes.size);
}

void Solver::onStartGuessing()
{
	interupt = false;
	guess->algorithm = rules->algorithm;
	start(QThread::NormalPriority);
}

void Solver::shuffle(QString &m_string) const
{
	qsrand(time(NULL));
	for (int i = 0; i < m_string.length(); i++) {
		int j = static_cast<int>(i *(qrand()/(RAND_MAX + 1.0)));
		QChar c = m_string.at(i);
		m_string.replace(i, 1, m_string.at(j));
		m_string.replace(j, 1, c);
	}
}

void Solver::permute(QString &m_code) const
{
	QString shuffled_colors = QString("0123456789").left(rules->colors);
	shuffle(shuffled_colors);
	shuffle(m_code);

	for(int i = 0; i < m_code.length(); ++i)
		m_code.replace(i, 1, shuffled_colors.at(m_code.at(i).digitValue()));
}

bool Solver::setResponse(const int &m_response)
{
	QList<int> temppossibles;
	int guessArray[rules->pegs];
	stringToArray(guess->guess, guessArray);

	int cmp;
	foreach(int possible, possibles) {
		compare(guessArray, codes.index[possible], rules->colors, rules->pegs, cmp);
		if( cmp == m_response)
			temppossibles.append(possible);
	}

	if (temppossibles.isEmpty())
		return false;

	possibles = temppossibles;
	guess->response = m_response;
	guess->possibles = possibles.size();
	setSmallPossibles();
	return true;
}

void Solver::setSmallPossibles()
{
	if (!sp.index)
	{
		if (codes.size <= 10000) {
			sp.size = codes.size;
			sp.index = new int[sp.size];
			for(int i = 0; i < sp.size; ++i)
				sp.index[i] = i;
		} else if (possibles.size() <= 10000) {
			sp.size = possibles.size();
			sp.index = new int[sp.size];
			for(int i = 0; i < sp.size; ++i)
				sp.index[i] = possibles.at(i);
		}
	}
}

void Solver::run()
{
	makeGuess();
	if (!interupt)
		emit guessDoneSignal();
}

QString Solver::getFirstGuess() const
{
	QString answer;
	QString str;
	if (rules->same_colors) {
		switch (rules->colors) {
		case 2:
			str = QString::fromUtf8("01010");
			answer = str.left(rules->pegs);
			break;
		case 3:
			str = QString::fromUtf8("01212");
			answer = str.left(rules->pegs);
			break;
		case 4:
			str = QString::fromUtf8("01223");
			answer = str.left(rules->pegs);
			break;
		default:
			str = QString::fromUtf8("01223");
			answer = str.left(rules->pegs);
			break;
		}

		// the classic game (c = 6, p = 4) is best with this first guess on Worst Case

		if(rules->colors == 6 && rules->pegs == 4 && rules->algorithm == Rules::Algorithm::WorstCase)
			answer = QString::fromUtf8("0011");
	} else {
		QString str = QString::fromUtf8("01234");
		answer = str.left(rules->pegs);
	}

	permute(answer);
	return answer;
}

void Solver::makeGuess()
{
	// The first guess here
	if (possibles.size() == codes.size) {
		guess->guess = getFirstGuess();
		return;
	}

	if (possibles.size() == 1) {
		guess->guess =  arrayToString(codes.index[possibles.first()]);
		return;
	}

	if(possibles.size() > 10000) {
		guess->guess = arrayToString(codes.index[possibles.at(possibles.size() >> 1)]);
		return;
	}

	int responsesOfCodes[max_response];
	for(int i = 0; i < max_response; ++i)
		responsesOfCodes[i] = 0;

	int answer_index = 0;
	qreal min_code_weight = 1000000000;
	qreal code_weight;

	for (int code_index = 0; code_index < sp.size; ++code_index) {
		if(interupt)
			return;

		int cmp;
		foreach(int possible_index, possibles) {
			compare(codes.index[sp.index[code_index]], codes.index[possible_index], rules->colors, rules->pegs, cmp);
			++responsesOfCodes[cmp];
		}
		code_weight = computeWeight(responsesOfCodes);

		if (code_weight < min_code_weight) {
			answer_index = code_index;
			min_code_weight = code_weight;
		}
	}

	if(guess->algorithm == Rules::Algorithm::MostParts)
		min_code_weight = max_response - 2 - min_code_weight;

	guess->weight = qFloor(min_code_weight);

	guess->guess = arrayToString(codes.index[sp.index[answer_index]]);
}

qreal Solver::computeWeight(int *m_responses) const
{
	qreal answer = 0;

	switch (guess->algorithm) {
	case Rules::Algorithm::ExpectedSize:
		for(int i = 0; i < max_response-2; ++i) {
			answer += ipow(m_responses[i], 2);
			m_responses[i] = 0;
		}
		break;
	case Rules::Algorithm::WorstCase:
		for(int i = 0; i < max_response-2; ++i) {
			answer = qMax((qreal)m_responses[i], answer);
			m_responses[i] = 0;
		}
		break;
	default:	//	Most Parts
		for(int i = 0; i < max_response-2; ++i) {
			if (m_responses[i] == 0)
				++answer;
			else
				m_responses[i] = 0;
		}
		break;
	}

	// This little trick will prefer possibles in tie
	if (m_responses[max_response - 1] != 0) {
		answer -= 0.5;
		if (guess->algorithm == Rules::Algorithm::MostParts)
			--answer;
		else
			++answer;

		m_responses[max_response - 1] = 0;
	}

	return answer;
}

void Solver::indexToCodeSameColor(const int &number)
{
	int m_number = number;
	for(int i = 0; i < rules->pegs; ++i) {
		codes.index[number][i] = m_number % rules->colors;
		m_number /= rules->colors;
	}
}

void Solver::indexToCodeDifferentColor(const int &number)
{
	QString NUMS = "0123456789"; // Characters that may be used
	int m_number = number;
	int max_digits = codes.size;
	for(int i = 0; i < rules->pegs; ++i) {
		max_digits /= rules->colors - i;
		int temp = m_number / max_digits;
		codes.index[number][i] = NUMS[temp].digitValue();
		NUMS.remove(temp, 1);
		m_number -= temp*max_digits;
	}
}

QString Solver::arrayToString(const int *m_array) const
{
	QString answer = "";
	for(int i = 0; i < rules->pegs; ++i)
		answer.append(QString::number(m_array[i]));
	return answer;
}

void Solver::stringToArray(const QString &m_string, int *m_array) const
{
	for(int i = 0; i < rules->pegs; ++i)
		m_array[i] = m_string[i].digitValue();
}
