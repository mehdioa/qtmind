/***********************************************************************
 *
 * Copyright (C) 2013 Mehdi Omidal <mehdioa@gmail.com>
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


Pin::Pin(const int &color, QGraphicsItem* parent) :
	QGraphicsEllipseItem(0, 0, 12, 12, parent),
	m_color(color)
{
	setPen(Qt::NoPen);
	QLinearGradient lgrad(0, 0, 12, 12);
	lgrad.setColorAt(0.0, QColor(0, 0, 0, 0x80));
	lgrad.setColorAt(1.0, QColor(0xff, 0xff, 0xff, 0xa0));
	setPen(QPen(QBrush(lgrad), 1));

	setMouseEventHandling(MOUSE_IGNORE);

}

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

void Pin::setMouseEventHandling(PIN_MOUSE event)
{
	switch (event) {
	case MOUSE_IGNORE:
		setEnabled(false);
		setAcceptedMouseButtons(Qt::NoButton);
		setCursor(Qt::ArrowCursor);
		break;
	case MOUSE_ACCEPT:
		setEnabled(true);
		setAcceptedMouseButtons(Qt::LeftButton);
		setCursor(Qt::PointingHandCursor);
		break;
	default: // MOUSE_TOBOX, let the keybox receive the mouse event
		setEnabled(false);
		setAcceptedMouseButtons(Qt::NoButton);
		setCursor(Qt::PointingHandCursor);
		break;
	}
}


void Pin::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (isEnabled())  setColor((m_color+2)%3 -1);

	QGraphicsEllipseItem::mousePressEvent(event);
}
