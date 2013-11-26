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
#include "constants.h"

class Pin : public QGraphicsEllipseItem
{
public:
	explicit Pin(const int &color = 0, QGraphicsItem* parent = 0);
	int getColor() const {return m_color;}
	void setColor(const int& c);
	void setMouseEventHandling(const PinMouse& event);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *);

	//	ignoring mouse double click by the following apparently on Qt >= 5
//	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *){}

private:
	int m_color;//  -1 = white, 0 = none, 1 = black
};

#endif // PIN_H
