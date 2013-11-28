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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QColor>

const int MIN_COLOR_NUMBER	= 2;
const int MAX_COLOR_NUMBER	= 10;

const int MIN_SLOT_NUMBER	= 2;
const int MAX_SLOT_NUMBER	= 5;

enum class Algorithm
{
	MostParts,
	WorstCase,
	ExpectedSize
};

enum class IndicatorType
{
	None,
	Character,
	Digit
};

enum class BoxState
{
	Past,
	Current,
	Future,
	None
};

const int PinPositions[MAX_SLOT_NUMBER][MAX_SLOT_NUMBER][2] =
{
	{{5, 14}, {22, 14}, {0, 0}, {0, 0}, {0, 0}},
	{{4, 7}, {22, 7}, {14, 21}, {0, 0}, {0, 0}},
	{{6, 6}, {22, 6}, {6, 22}, {22, 22}, {0, 0}},
	{{4, 4}, {24, 4}, {14, 14}, {4, 24}, {24, 24}}
};

#endif // CONSTANTS_H
