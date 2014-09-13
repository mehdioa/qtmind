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

#ifndef RULES_H
#define RULES_H

#include "appinfo.h"
/**
 * @brief The Rules class represents the rules of the game
 */
class Rules
{

public:

	static Rules *instance();
	~Rules();

	void update(int _colors, int _pegs, Algorithm _alg, Mode _mode, bool _same_color);
	void setAlgorithm(int a);
	int getPegs() const { return pegs; }
	int getColors() const { return colors; }
	Algorithm getAlgorithm() const { return algorithm; }
	bool isSameColor() const { return same_colors; }
	Mode getMode() const { return mode;}
private:
	Rules();

private:
	int pegs; /**< TODO */
	int colors; /**< TODO */
	bool same_colors; /**< TODO */
	Algorithm algorithm; /**< TODO */
	Mode mode; /**< TODO */
};

#endif // RULES_H
