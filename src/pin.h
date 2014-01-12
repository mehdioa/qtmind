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

#ifndef PIN_H
#define PIN_H

#include <QGraphicsEllipseItem>

enum class PinMouseState
{
	Accept,
	Ignore,
	PassToBox
};

enum class PinColor
{
	White,
	Black,
	None,
};

class Pin : public QGraphicsEllipseItem
{
public:
	explicit Pin(QGraphicsItem *parent = 0);
	PinColor getColor() const {return color;}
	void setColor(const PinColor &m_color);
	void setMouseState (const PinMouseState &m_state);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);

private:
	PinColor nextPinColor() const;

private:
	PinColor color;
//	int color;//  -1 = white, 0 = none, 1 = black
	PinMouseState pinMouseState;
};

#endif // PIN_H
