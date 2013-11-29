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

#include "pin.h"
#include <QPen>
#include <QCursor>

Pin::Pin(const int &color, QGraphicsItem *parent) :
	QGraphicsEllipseItem(0, 0, 12, 12, parent),
	m_color(color),
	onDemand(false)
{
	setPen(Qt::NoPen);
	QLinearGradient lgrad(0, 0, 12, 12);
	lgrad.setColorAt(0.0, QColor(0, 0, 0, 128));
	lgrad.setColorAt(1.0, QColor(255, 255, 255, 160));
	setPen(QPen(QBrush(lgrad), 1));

	setMouseEventHandling(PinMouse::Ignore);
}
//-----------------------------------------------------------------------------

void Pin::mousePressEvent(QGraphicsSceneMouseEvent *)
{
	if (onDemand)
		setColor((m_color - 2) % 3 + 1);
}
//-----------------------------------------------------------------------------

void Pin::setColor(const int &c)
{
	if (-2 < c && c < 2) m_color = c;

	switch (m_color) {
	case -1:
		setBrush(Qt::white);
		break;
	case 1:
		setBrush(Qt::black);
		break;
	default:
		setBrush(Qt::NoBrush);
		break;
	}
	update();
}
//-----------------------------------------------------------------------------

void Pin::setMouseEventHandling(const PinMouse &event)
{
	switch (event) {
	case PinMouse::Ignore:
		onDemand = false;
		setAcceptedMouseButtons(Qt::NoButton);
		setCursor(Qt::ArrowCursor);
		break;
	case PinMouse::Accept:
		onDemand = true;
		setAcceptedMouseButtons(Qt::LeftButton);
		setCursor(Qt::PointingHandCursor);
		break;
	default: // PinMouse::Pass, let the pinbox receive the mouse event
		onDemand = false;
		setAcceptedMouseButtons(Qt::NoButton);
		setCursor(Qt::PointingHandCursor);
		break;
	}
}
