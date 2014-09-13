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
	interupt(true)
{
	codes.index = NULL;
	sp.index = NULL;
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
	if (Rules::instance()->isSameColor()) {
		codes.size = ipow(Rules::instance()->getColors(), Rules::instance()->getPegs());
	} else {
		codes.size = 1;
		for(int i = 0; i < Rules::instance()->getPegs(); ++i)
			codes.size *= (Rules::instance()->getColors() - i);
	}

	max_response = (Rules::instance()->getPegs() + 1)*(Rules::instance()->getPegs() + 2)/2;

	codes.index = new unsigned char *[codes.size];

	for(int i = 0; i < codes.size; ++i) {
		codes.index[i] = new unsigned char[Rules::instance()->getPegs()];
	}

	if (Rules::instance()->isSameColor()) {
		for (int i = 0; i < Rules::instance()->getPegs(); i++)
			codes.index[0][i] = 0;
		for (int i = 1; i < codes.size; i++) {
			array_copy(codes.index[i-1], codes.index[i], Rules::instance()->getPegs());
			indexToCodeSameColor(codes.index[i]);
		}
	} else {
		for (int i = 0; i < Rules::instance()->getPegs(); i++)
			codes.index[0][i] = i;
		for (int i = 1; i < codes.size; i++) {
			array_copy(codes.index[i-1], codes.index[i], Rules::instance()->getPegs());
			indexToCodeDifferentColor(codes.index[i]);
		}
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
	Guess::instance()->reset(codes.size);
}

void Solver::onStartGuessing()
{
	interupt = false;
	Guess::instance()->algorithm = Rules::instance()->getAlgorithm(); // to prevent change in algorithm by user in the middle of computation
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
	shuffle(shuffled_colors, Rules::instance()->getColors());
	shuffle(m_code, Rules::instance()->getPegs());

	for(int i = 0; i < Rules::instance()->getPegs(); ++i)
		m_code[i] = shuffled_colors[m_code[i]];
}

bool Solver::setResponse(const int &blacks, const int &whites)
{
	QList<int> temppossibles;
	int _blacks, _whites;
	foreach(int possible, possibles) {
		COMPARE(Guess::instance()->guess, codes.index[possible], Rules::instance()->getColors(), Rules::instance()->getPegs(), _blacks, _whites);
		if (blacks == _blacks && whites == _whites)
			temppossibles.append(possible);
	}

	if (temppossibles.isEmpty())
		return false;
	possibles = temppossibles;
	Guess::instance()->blacks = blacks;
	Guess::instance()->whites = whites;
	Guess::instance()->possibles = possibles.size();
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
		if (Rules::instance()->isSameColor()) {
			switch (Rules::instance()->getColors()) {
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
			if(Rules::instance()->getColors() == 6 && Rules::instance()->getPegs() == 4 && Rules::instance()->getAlgorithm() == Algorithm::WorstCase) {
				answer[2] = 0;
				answer[3] = 1;
			}
		}
		permute(answer);
		Guess::instance()->setGuess(answer);
		return;
	}

	if (possibles.size() == 1) {
		Guess::instance()->setGuess(codes.index[possibles.first()]);
		return;
	}

	if(possibles.size() > 10000) {
		Guess::instance()->setGuess(codes.index[possibles.at(possibles.size() >> 1)]);
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
			COMPARE(codes.index[sp.index[code_index]], codes.index[possible_index], Rules::instance()->getColors(), Rules::instance()->getPegs(), blacks, whites);
			++responsesOfCodes[(blacks+whites)*(blacks+whites+1)/2 + blacks];
		}
		code_weight = computeWeight(responsesOfCodes);

		if (code_weight < min_code_weight) {
			answer_index = code_index;
			min_code_weight = code_weight;
		}
	}

	if(Guess::instance()->algorithm == Algorithm::MostParts)
		min_code_weight = max_response - 2 - min_code_weight;

	Guess::instance()->weight = qFloor(min_code_weight);

	Guess::instance()->setGuess(codes.index[sp.index[answer_index]]);
}

qreal Solver::computeWeight(int *m_responses) const
{
	qreal answer = 0;

	switch (Guess::instance()->algorithm) {
	case Algorithm::ExpectedSize:
		for(int i = 0; i < max_response-2; ++i) {
			answer += ipow(m_responses[i], 2);
			m_responses[i] = 0;
		}
		break;
	case Algorithm::WorstCase:
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
		if (Guess::instance()->algorithm == Algorithm::MostParts)
			--answer;
		else
			++answer;

		m_responses[max_response - 1] = 0;
	}

	return answer;
}

void Solver::indexToCodeSameColor(unsigned char *X)
{
	int i = Rules::instance()->getPegs() - 1;
	int N = Rules::instance()->getColors();
	N--;
	while (i >= 0 && X[i] >= N)
		X[i--] = 0;
	if (i < 0) return;
	X[i]++;
}

void Solver::indexToCodeDifferentColor(unsigned char *X)
{
	int i = Rules::instance()->getPegs();
	while (--i >= 0) {
		while (++X[i] < Rules::instance()->getColors()) {
			int j = 0;
			while (j < i && X[j] != X[i])
				++j;
			if (j == i) goto mark;
		}
	}
	return;
mark:
	while (++i < Rules::instance()->getPegs()) {
		X[i] = -1;
		while (++X[i] < Rules::instance()->getColors()) {
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
	for(int i = 0; i < Rules::instance()->getPegs(); ++i) {
		answer.append(QString::number(m_array[i]));
	}
	return answer;
}
