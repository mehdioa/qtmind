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

#ifndef INDICATOR_H
#define INDICATOR_H

enum class IndicatorType
{
	Character,
	Digit
};


class Indicator
{
public:
	Indicator();
	~Indicator();
	bool forceColor() const;
	void setShowColors(const bool &show_colors);
	void setShowIndicators(const bool &show_indicators);
	void setIndicatorType(const IndicatorType &indicator_type);
	bool getShowColors() const;
	bool getShowIndicators() const;
	IndicatorType getIndicatorType() const;

private:
	bool showColors;
	bool showIndicators;
	IndicatorType indicatorType;
};

#endif // INDICATOR_H
