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
#include "rules.h"
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

Solver::Solver(QObject *parent):
	QThread(parent),
	m_interupt(true)
{
	m_codes.index = NULL;
	m_smallPossibles.index = NULL;
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
	if (Rules::instance()->m_sameColors) {
		m_codes.size = ipow(Rules::instance()->m_colors, Rules::instance()->m_pegs);
	} else {
		m_codes.size = 1;
		for(int i = 0; i < Rules::instance()->m_pegs; ++i)
			m_codes.size *= (Rules::instance()->m_colors - i);
	}

	m_maxResponse = (Rules::instance()->m_pegs + 1)*(Rules::instance()->m_pegs + 2)/2;

	m_codes.index = new unsigned char *[m_codes.size];

	for(int i = 0; i < m_codes.size; ++i) {
		m_codes.index[i] = new unsigned char[Rules::instance()->m_pegs];
	}

	if (Rules::instance()->m_sameColors) {
		for (int i = 0; i < Rules::instance()->m_pegs; i++)
			m_codes.index[0][i] = 0;
		for (int i = 1; i < m_codes.size; i++) {
			array_copy(m_codes.index[i-1], m_codes.index[i], Rules::instance()->m_pegs);
			indexToCodeSameColor(m_codes.index[i]);
		}
	} else {
		for (int i = 0; i < Rules::instance()->m_pegs; i++)
			m_codes.index[0][i] = i;
		for (int i = 1; i < m_codes.size; i++) {
			array_copy(m_codes.index[i-1], m_codes.index[i], Rules::instance()->m_pegs);
			indexToCodeDifferentColor(m_codes.index[i]);
		}
	}

	for(int i = 0; i < m_codes.size; ++i)
		m_possibles.append(i);
}

void Solver::deleteTables()
{
	if (m_codes.index != NULL) {
		for(int i = 0; i < m_codes.size; ++i)
			delete[] m_codes.index[i];

		delete[] m_codes.index;
		m_codes.index = NULL;
	}

	if(m_smallPossibles.index) {
		delete[] m_smallPossibles.index;
		m_smallPossibles.index = NULL;
	}

	m_possibles.clear();
}

void Solver::onReset()
{
	deleteTables();
	createTables();
	Guess::instance()->reset(m_codes.size);
}

void Solver::onStartGuessing()
{
	m_interupt = false;
	Guess::instance()->m_algorithm = Rules::instance()->m_algorithm; // to prevent change in algorithm by user in the middle of computation
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
	shuffle(shuffled_colors, Rules::instance()->m_colors);
	shuffle(m_code, Rules::instance()->m_pegs);

	for(int i = 0; i < Rules::instance()->m_pegs; ++i)
		m_code[i] = shuffled_colors[m_code[i]];
}

bool Solver::setResponse(const int &blacks, const int &whites)
{
	QList<int> temppossibles;
	int _blacks, _whites;
	foreach(int possible, m_possibles) {
		COMPARE(Guess::instance()->m_guess, m_codes.index[possible], Rules::instance()->m_colors, Rules::instance()->m_pegs, _blacks, _whites);
		if (blacks == _blacks && whites == _whites)
			temppossibles.append(possible);
	}

	if (temppossibles.isEmpty())
		return false;
	m_possibles = temppossibles;
	Guess::instance()->m_blacks = blacks;
	Guess::instance()->m_whites = whites;
	Guess::instance()->m_possibles = m_possibles.size();
	setSmallPossibles();
	return true;
}

void Solver::setSmallPossibles()
{
	if (!m_smallPossibles.index)
	{
		if (m_codes.size <= 10000) {
			m_smallPossibles.size = m_codes.size;
			m_smallPossibles.index = new int[m_smallPossibles.size];
			for(int i = 0; i < m_smallPossibles.size; ++i)
				m_smallPossibles.index[i] = i;
		} else if (m_possibles.size() <= 10000) {
			m_smallPossibles.size = m_possibles.size();
			m_smallPossibles.index = new int[m_smallPossibles.size];
			for(int i = 0; i < m_smallPossibles.size; ++i)
				m_smallPossibles.index[i] = m_possibles.at(i);
		}
	}
}

void Solver::run()
{
	makeGuess();
	if (!m_interupt)
		emit guessDoneSignal();
}

void Solver::makeGuess()
{
	// The first guess here
	if (m_possibles.size() == m_codes.size) {
		unsigned char answer[] = {0, 1, 2, 3, 4};
		if (Rules::instance()->m_sameColors) {
			switch (Rules::instance()->m_colors) {
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
			if(Rules::instance()->m_colors == 6 && Rules::instance()->m_pegs == 4 && Rules::instance()->m_algorithm == Algorithm::WorstCase) {
				answer[2] = 0;
				answer[3] = 1;
			}
		}
		permute(answer);
		Guess::instance()->setGuess(answer);
		return;
	}

	if (m_possibles.size() == 1) {
		Guess::instance()->setGuess(m_codes.index[m_possibles.first()]);
		return;
	}

	if(m_possibles.size() > 10000) {
		Guess::instance()->setGuess(m_codes.index[m_possibles.at(m_possibles.size() >> 1)]);
		return;
	}

	int responsesOfCodes[m_maxResponse];
	for(int i = 0; i < m_maxResponse; ++i)
		responsesOfCodes[i] = 0;

	int answer_index = 0;
	qreal min_code_weight = 1000000000;
	qreal code_weight;

	for (int code_index = 0; code_index < m_smallPossibles.size; ++code_index) {
		if(m_interupt)
			return;

		int whites, blacks;
		foreach(int possible_index, m_possibles) {
			COMPARE(m_codes.index[m_smallPossibles.index[code_index]], m_codes.index[possible_index], Rules::instance()->m_colors, Rules::instance()->m_pegs, blacks, whites);
			++responsesOfCodes[(blacks+whites)*(blacks+whites+1)/2 + blacks];
		}
		code_weight = computeWeight(responsesOfCodes);

		if (code_weight < min_code_weight) {
			answer_index = code_index;
			min_code_weight = code_weight;
		}
	}

	if(Guess::instance()->m_algorithm == Algorithm::MostParts)
		min_code_weight = m_maxResponse - 2 - min_code_weight;

	Guess::instance()->m_weight = qFloor(min_code_weight);

	Guess::instance()->setGuess(m_codes.index[m_smallPossibles.index[answer_index]]);
}

qreal Solver::computeWeight(int *m_responses) const
{
	qreal answer = 0;

	switch (Guess::instance()->m_algorithm) {
	case Algorithm::ExpectedSize:
		for(int i = 0; i < m_maxResponse-2; ++i) {
			answer += ipow(m_responses[i], 2);
			m_responses[i] = 0;
		}
		break;
	case Algorithm::WorstCase:
		for(int i = 0; i < m_maxResponse-2; ++i) {
			answer = qMax((qreal)m_responses[i], answer);
			m_responses[i] = 0;
		}
		break;
	default:	//	Most Parts
		for(int i = 0; i < m_maxResponse-2; ++i) {
			if (m_responses[i] == 0)
				++answer;
			else
				m_responses[i] = 0;
		}
		break;
	}

	// This little trick will prefer possibles in tie
	if (m_responses[m_maxResponse - 1] != 0) {
		answer -= 0.5;
		if (Guess::instance()->m_algorithm == Algorithm::MostParts)
			--answer;
		else
			++answer;

		m_responses[m_maxResponse - 1] = 0;
	}

	return answer;
}

void Solver::indexToCodeSameColor(unsigned char *X)
{
	int i = Rules::instance()->m_pegs - 1;
	int N = Rules::instance()->m_colors;
	N--;
	while (i >= 0 && X[i] >= N)
		X[i--] = 0;
	if (i < 0) return;
	X[i]++;
}

void Solver::indexToCodeDifferentColor(unsigned char *X)
{
	int i = Rules::instance()->m_pegs;
	while (--i >= 0) {
		while (++X[i] < Rules::instance()->m_colors) {
			int j = 0;
			while (j < i && X[j] != X[i])
				++j;
			if (j == i) goto mark;
		}
	}
	return;
mark:
	while (++i < Rules::instance()->m_pegs) {
		X[i] = -1;
		while (++X[i] < Rules::instance()->m_colors) {
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
	for(int i = 0; i < Rules::instance()->m_pegs; ++i) {
		answer.append(QString::number(m_array[i]));
	}
	return answer;
}
