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

enum GAME_MODE {
	MODE_MASTER		= 0,//HUMAN BREAKS THE CODE
	MODE_BREAKER	= 1	//MACHINE BREAKS THE CODE
};

enum GAME_STATE {
	STATE_NONE						= 0,
	STATE_RUNNING					= 1,
	STATE_PAUSED					= 2,
	STATE_WON						= 3,
	STATE_LOST						= 4,
	STATE_GAVE_IN					= 5,
	STATE_ERROR						= 6,
	STATE_GUESSING					= 7,
	STATE_WAITING_FOR_PINS			= 8,
	STATE_WAITING_FOR_MASTER_CODE	= 9,
	STATE_WAITING_FOR_CODE_ROW		=10
};


enum Algorithm {
	MostParts		= 0,
	WorstCase		= 1,
	ExpectedSize	= 2
};


const int MIN_COLOR_NUMBER	= 2;
const int MAX_COLOR_NUMBER	= 10;

const int MIN_SLOT_NUMBER	= 2;
const int MAX_SLOT_NUMBER	= 5;

const int MAX_ROW_NUMBER	= 10;



#endif // CONSTANTS_H
