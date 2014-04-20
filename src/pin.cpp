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

Pin::Pin(QGraphicsItem *parent) :
	QGraphicsEllipseItem(0, 0, 13, 13, parent),
	color(PinColor::None),
	activity(false)
{
	QLinearGradient lgrad(0, 0, 0, 13);
	lgrad.setColorAt(0.0, QColor(50, 50, 50));
	lgrad.setColorAt(1.0, QColor(220, 220, 220));
	setPen(QPen(QBrush(lgrad), 1));

	setAcceptedMouseButtons(Qt::NoButton);
	setCursor(Qt::ArrowCursor);
}
//-----------------------------------------------------------------------------

void Pin::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsEllipseItem::mousePressEvent(event);
	if (activity)
		setColor(nextPinColor());
}
//-----------------------------------------------------------------------------

void Pin::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
#if QT_VERSION < 0x050000
	QGraphicsEllipseItem::mouseDoubleClickEvent(event);
#else
	Q_UNUSED(event);
#endif
}
//-----------------------------------------------------------------------------

PinColor Pin::nextPinColor() const
{
	switch (color) {
	case PinColor::Black:
		return PinColor::None;
	case PinColor::White:
		return PinColor::Black;
	default:
		return PinColor::White;
	}
}
//-----------------------------------------------------------------------------

void Pin::setColor(const PinColor &m_color)
{
	color = m_color;
	switch (color) {
	case PinColor::White:
		setBrush(Qt::white);
		break;
	case PinColor::Black:
		setBrush(Qt::black);
		break;
	default:
		setBrush(Qt::NoBrush);
		break;
	}
}
//-----------------------------------------------------------------------------

void Pin::setActivity(const bool &b)
{
	activity = b;
	if (activity) {
		setAcceptedMouseButtons(Qt::LeftButton);
		setCursor(Qt::PointingHandCursor);
	} else {
		setAcceptedMouseButtons(Qt::NoButton);
		setCursor(Qt::ArrowCursor);
	}
}
