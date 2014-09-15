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

#include "guess.h"
#include <QDebug>

Guess::Guess()
{
	reset(0);
}

Guess *Guess::instance()
{
	static Guess g;
	return &g;
}

void Guess::reset(const int &_possibles)
{
	m_colors = Rules::instance()->m_colors;
	m_pegs = Rules::instance()->m_pegs;
	std::fill(m_guess, m_guess + MAX_SLOT_NUMBER, 0);
	std::fill(m_code, m_code + MAX_SLOT_NUMBER, 0);
	m_blacks = 0;
	m_whites = 0;
	m_algorithm = Rules::instance()->m_algorithm;
	m_possibles = _possibles;
	m_weight = 0;
}

void Guess::setGuess(unsigned char *_guess)
{
	for(int i = 0; i < MAX_SLOT_NUMBER; i++)
		m_guess[i] = _guess[i];
	COMPARE(m_code, m_guess, m_colors, m_pegs, m_blacks, m_whites);
}

void Guess::setCode(unsigned char *_code)
{
	for(int i = 0; i < MAX_SLOT_NUMBER; i++)
		m_code[i] = _code[i];
}
