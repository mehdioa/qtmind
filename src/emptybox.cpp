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

#include "emptybox.h"
#include <QPen>
#include <QBrush>
#include <QPainter>

const int EmptyBox::colors_rgb[4] =
{
	204, 252,  //left-bottom, right-top colors
	236, 100  // left-bottom, right-top gradient
};
//-----------------------------------------------------------------------------

const int EmptyBox::alphas[4] =
{
	160, 255, 50, 160
};
//-----------------------------------------------------------------------------

EmptyBox::EmptyBox(const QPoint &position, QGraphicsItem *parent):
	QGraphicsRectItem(0, 0, 39, 39, parent),
	mBoxState(BoxState::Future)
{
	setZValue(1);
	setPos(position);
	setFlag(QGraphicsItem::ItemIsMovable, false);
}
//-----------------------------------------------------------------------------

void EmptyBox::setBoxState(const BoxState &state)
{
	mBoxState = state;
}
//-----------------------------------------------------------------------------

void EmptyBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(Qt::NoPen);

	QRadialGradient grad(QPoint(10, 10), 100);
	grad.setColorAt(0, QColor(colors_rgb[0], colors_rgb[0], colors_rgb[0], alphas[(int)mBoxState]));
	grad.setColorAt(1, QColor(colors_rgb[1], colors_rgb[1], colors_rgb[1], alphas[(int)mBoxState]));
	painter->setBrush(QBrush(grad));
	painter->drawRect(0, 0, 39, 39);

	painter->setPen(QPen(QBrush(QColor(colors_rgb[2], colors_rgb[2], colors_rgb[2], alphas[(int)mBoxState])),1));
	painter->drawLine(0, 0, 0, 39);
	painter->drawLine(0, 0, 39, 0);
	painter->drawLine(0, .5, 39, .5);
	painter->drawLine(.5, 0, .5, 39);

	painter->setPen(QPen(QBrush(QColor(colors_rgb[3], colors_rgb[3], colors_rgb[3], alphas[(int)mBoxState])),1));
	painter->drawLine(39, 0, 39, 39);
	painter->drawLine(0, 39, 39, 39);
	painter->drawLine(39.5, 0, 39.5, 39);
	painter->drawLine(0, 39.5, 39, 39.5);
}
