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


enum class GameMode {
	Master,		//HUMAN BREAKS THE CODE
	Breaker		//MACHINE BREAKS THE CODE
};

enum class GameState
{
	None,
	Running,
	Paused,
	Win,
	Lose,
	Resign,
	Error,
	Guessing,
	WaittingOkButtonPress,
	WaittingDoneButtonPress,
	WaittingFirstRowFill,
	WaittingPinBoxPress
};


enum class Algorithm
{
	MostParts,
	WorstCase,
	ExpectedSize
};

enum class PinMouse
{
	Ignore,
	Accept,
	Pass
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

enum class PegState
{
	Initial,
	Filled,
	Empty,
	Draged
};

const int BoxAlphas[4] =
{
	160, 255, 50, 160
};


const QColor PegColors[MAX_COLOR_NUMBER][2] = {
	{QColor("#FFFFBB"), QColor("#FFAA00")},
	{QColor("#FF0000"), QColor("#400040")},
	{QColor("#33CCFF"), QColor("#000080")},
	{QColor("#7F7F7F"), QColor("#000000")},
	{QColor("#FFFFFF"), QColor("#7F7F7F")},
	{QColor("#FF9900"), QColor("#A82A00")},
	{QColor("#66FF33"), QColor("#385009")},
	{QColor("#BA88FF"), QColor("#38005D")},
	{QColor("#00FFFF"), QColor("#004040")},
	{QColor("#FF82F3"), QColor("#800080")}
  };

const QString OrderedChars[3] = {"          ", "ABCDEFGHIJ", "0123456789"};

const int PinPositions[MAX_SLOT_NUMBER][MAX_SLOT_NUMBER][2] =
{
	{{5, 14}, {22, 14}, {0, 0}, {0, 0}, {0, 0}},
	{{4, 7}, {22, 7}, {14, 21}, {0, 0}, {0, 0}},
	{{6, 6}, {22, 6}, {6, 22}, {22, 22}, {0, 0}},
	{{4, 4}, {24, 4}, {14, 14}, {4, 24}, {24, 24}}
};

#endif // CONSTANTS_H
