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
	return (guess->blacks == rules->pegs);
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

	codes.index = new unsigned char *[codes.size];

	for(int i = 0; i < codes.size; ++i) {
		codes.index[i] = new unsigned char[rules->pegs];
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
	guess->algorithm = rules->algorithm; // to prevent change in algorithm by user in the middle of computation
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
	unsigned char shuffled_colors[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255};// QString("0123456789").left(rules->colors);
	shuffle(shuffled_colors, rules->colors);
	shuffle(m_code, rules->pegs);

	for(int i = 0; i < rules->pegs; ++i)
		m_code[i] = shuffled_colors[m_code[i]];
}

bool Solver::setResponse(const int &blacks, const int &whites)
{
	QList<int> temppossibles;
	int _blacks, _whites;
	foreach(int possible, possibles) {
		COMPARE(guess->guess, codes.index[possible], rules->colors, rules->pegs, _blacks, _whites);
		if (blacks == _blacks && whites == _whites)
			temppossibles.append(possible);
	}

	if (temppossibles.isEmpty())
		return false;
	possibles = temppossibles;
	guess->blacks = blacks;
	guess->whites = whites;
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

void Solver::makeGuess()
{
	// The first guess here
	if (possibles.size() == codes.size) {
		unsigned char answer[] = {0, 1, 2, 3, 4};
		if (rules->same_colors) {
			switch (rules->colors) {
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
			if(rules->colors == 6 && rules->pegs == 4 && rules->algorithm == Rules::Algorithm::WorstCase) {
				answer[2] = 0;
				answer[3] = 1;
			}
		}
		permute(answer);
		guess->setGuess(answer);
		return;
	}

	if (possibles.size() == 1) {
		guess->setGuess(codes.index[possibles.first()]);
		return;
	}

	if(possibles.size() > 10000) {
		guess->setGuess(codes.index[possibles.at(possibles.size() >> 1)]);
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

		int whites, blacks;
		foreach(int possible_index, possibles) {
			COMPARE(codes.index[sp.index[code_index]], codes.index[possible_index], rules->colors, rules->pegs, blacks, whites);
			++responsesOfCodes[(blacks+whites)*(blacks+whites+1)/2 + blacks];
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

	guess->setGuess(codes.index[sp.index[answer_index]]);
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
		codes.index[number][i] = static_cast<char>(m_number % rules->colors);
		m_number /= rules->colors;
	}
}

void Solver::indexToCodeDifferentColor(const int &number)
{
//	unsigned char NUMS[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255};
	int m_number = number;
	int max_digits = codes.size;
	for(int i = 0; i < rules->pegs; ++i) {
		max_digits /= rules->colors - i;
		int temp = m_number / max_digits;
		m_number -= temp*max_digits;
		for(int j = 0; j < i; j++)
			if (codes.index[number][j] <= temp)
				temp++;
		codes.index[number][i] = temp;
	}
}

QString Solver::arrayToString(const unsigned char *m_array) const
{
	QString answer = "";
	for(int i = 0; i < rules->pegs; ++i) {
		answer.append(QString::number(m_array[i]));
	}
	return answer;
}
