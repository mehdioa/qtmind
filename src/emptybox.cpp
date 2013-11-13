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

#include "emptybox.h"
#include <QPen>
#include <QBrush>
#include <QPainter>

const int EmptyBox::colors_rgb[4][3] =
{
	{204, 205, 207}, {252, 253, 255},  //left-bottom, right-top colors
	{236, 237, 239}, {100, 101, 104}  // left-bottom, right-top gradient
};
//-----------------------------------------------------------------------------

const int EmptyBox::alphas[4] =
{
	0xa0, 0xff, 0x32, 0xa0
};
//-----------------------------------------------------------------------------

EmptyBox::EmptyBox(const QPoint &position, QGraphicsItem *parent):
	QGraphicsItem(parent),
	mBoxState(BOX_FUTURE)
{
	setZValue(1);
	setPos(position);
	setFlag(QGraphicsItem::ItemIsMovable, false);
}
//-----------------------------------------------------------------------------

void EmptyBox::setBoxState(const BOX_STATE& state)
{
	mBoxState = state;
}
//-----------------------------------------------------------------------------

QRectF EmptyBox::boundingRect() const
{
	return QRect(0, 0, 40, 40);
}
//-----------------------------------------------------------------------------

void EmptyBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(Qt::NoPen);

	QRadialGradient grad(QPoint(10, 10), 100);
	grad.setColorAt(0, QColor(colors_rgb[0][0], colors_rgb[0][1], colors_rgb[0][2], alphas[mBoxState]));
	grad.setColorAt(1, QColor(colors_rgb[1][0], colors_rgb[1][1], colors_rgb[1][2], alphas[mBoxState]));
	painter->setBrush(QBrush(grad));
	painter->drawRect(0, 0, 39, 39);

	painter->setPen(QPen(QBrush(QColor(colors_rgb[2][0], colors_rgb[2][1], colors_rgb[2][2], alphas[mBoxState])),1));
	painter->drawLine(0, 0, 0, 39);
	painter->drawLine(0, 0, 39, 0);

	painter->setPen(QPen(QBrush(QColor(colors_rgb[3][0], colors_rgb[3][1], colors_rgb[3][2], alphas[mBoxState])),1));
	painter->drawLine(39, 0, 39, 39);
	painter->drawLine(0, 39, 39, 39);
}
