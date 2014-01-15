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

#ifndef GAMERULES_H
#define GAMERULES_H

#include "constants.h"
#include <QLocale>

enum class GameMode {
	MVH,		//HUMAN MAKE THE HIDDEN CODE
	HVM		//HUMAN BREAKS THE HIDDEN CODE
};

class GameRules
{
	GameRules();
	~GameRules();

private:
	int pegNumber;
	int colorNumber;
	bool sameColorAllowed;
	Algorithm algorithm;
	GameMode gameMode;

	friend class MainWindow;
	friend class Solver;
	friend class Game;
};

#endif // GAMERULES_H
