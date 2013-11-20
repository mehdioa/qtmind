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

enum class GameMode {
	Master		= 0,//HUMAN BREAKS THE CODE
	Breaker		= 1	//MACHINE BREAKS THE CODE
};

enum class GameState {
	None						= 0,
	Running						= 1,
	Paused						= 2,
	Win							= 3,
	Lose						= 4,
	Resign						= 5,
	Error						= 6,
	Guessing					= 7,
	WaittingPins				= 8,
	WaittingMasterCode			= 9,
	WaittingCodeRow				=10
};


enum class Algorithm {
	MostParts		= 0,
	WorstCase		= 1,
	ExpectedSize	= 2
};

enum class PinMouse{
	Ignore		= 0,
	Accept		= 1,
	Pass		= 2
};

enum class IndicatorType{
	None	= 0,
	Charactre	= 1,
	Digit	= 2
};

enum class BoxState {
	Past		= 0,
	Current		= 1,
	Future		= 2,
	None		= 3
};

enum class PegState {
	Initial		= 0,
	Filled		= 1,
	Empty		= 2,
	Draged		= 3
};


const int MIN_COLOR_NUMBER	= 2;
const int MAX_COLOR_NUMBER	= 10;

const int MIN_SLOT_NUMBER	= 2;
const int MAX_SLOT_NUMBER	= 5;

const int MAX_ROW_NUMBER	= 10;



#endif // CONSTANTS_H
