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

#include "indicator.h"
#include <QSettings>

Indicator::Indicator()
{
	showColors = QSettings().value("ShowColors", 1).toBool();
	showIndicators =QSettings().value("ShowIndicators", 0).toBool();
	indicatorType = (IndicatorType) QSettings().value("IndicatorType", 0).toInt();
}
//-----------------------------------------------------------------------------

Indicator::~Indicator()
{
	QSettings().setValue("ShowColors", (int) showColors);
	QSettings().setValue("ShowIndicators", (int) showIndicators);
	QSettings().setValue("IndicatorType", (int) indicatorType);
}
//-----------------------------------------------------------------------------

bool Indicator::forceColor() const
{
	return showColors || !showIndicators;
}
//-----------------------------------------------------------------------------

void Indicator::setShowColors(const bool &show_colors)
{
	showColors = show_colors;
}
//-----------------------------------------------------------------------------

void Indicator::setShowIndicators(const bool &show_indicators)
{
	showIndicators = show_indicators;
}
//-----------------------------------------------------------------------------

void Indicator::setIndicatorType(const IndicatorType &indicator_type)
{
	indicatorType = indicator_type;
}
//-----------------------------------------------------------------------------

bool Indicator::getShowColors() const
{
	return showColors;
}
//-----------------------------------------------------------------------------

bool Indicator::getShowIndicators() const
{
	return showIndicators;
}
//-----------------------------------------------------------------------------

IndicatorType Indicator::getIndicatorType() const
{
	return indicatorType;
}
