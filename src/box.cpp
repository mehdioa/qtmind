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

#include "box.h"
#include <QPen>
#include <QBrush>
#include <QPainter>

const int Box::BoxAlphas[4] =
{
	210, 255, 80, 210
};

Box::Box(const QPoint &position, QGraphicsItem *parent):
	QGraphicsRectItem(0, 0, 39, 39, parent),
	state(Box::State::Future)
{
	setZValue(1);
	setPos(position);
	setFlag(QGraphicsItem::ItemIsMovable, false);
}

void Box::setState(const Box::State &m_state)
{
	state = m_state;
}

void Box::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(Qt::NoPen);

	painter->setBrush(QBrush(QColor(225, 225, 225, BoxAlphas[(int)state])));
	painter->drawRect(0, 0, 39, 39);

	painter->setPen(QPen(QBrush(QColor(236, 236, 236, BoxAlphas[(int)state])),1));
	painter->drawLine(0, 0, 0, 39);
	painter->drawLine(0, 0, 39, 0);
	painter->drawLine(QPointF(0, .5), QPointF(39, .5));
	painter->drawLine(QPointF(.5, 0), QPointF(.5, 39));

	painter->setPen(QPen(QBrush(QColor(80, 80, 80, BoxAlphas[(int)state])),1));
	painter->drawLine(39, 0, 39, 39);
	painter->drawLine(0, 39, 39, 39);
	painter->drawLine(QPointF(39.5, 0), QPointF(39.5, 39));
	painter->drawLine(QPointF(0, 39.5), QPointF(39, 39.5));
}
