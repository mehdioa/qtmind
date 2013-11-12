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


EmptyBox::EmptyBox(const QPoint &position, QGraphicsItem *parent):
	QGraphicsItem(parent),
	m_state(BOX_FUTURE)
{

	mPend	= QColor("#646568");
	mPenl	= QColor("#ecedef");
	mGrad0	= QColor("#cccdcf");
	mGrad1	= QColor("#fcfdff");

	setHighlight();

	setZValue(1);
	setPos(position);
	setFlag(QGraphicsItem::ItemIsMovable, false);
}

void EmptyBox::setState(const BOX_STATE& state)
{
	m_state = state;
	setHighlight();
}

void EmptyBox::setHighlight()
{
	switch (m_state) {
	case BOX_PAST:
		mPend.setAlpha(0xa0);
		mPenl.setAlpha(0xa0);
		mGrad0.setAlpha(0xa0);
		mGrad1.setAlpha(0xa0);
		break;
	case BOX_CURRENT:
		mPend.setAlpha(0xff);
		mPenl.setAlpha(0xff);
		mGrad0.setAlpha(0xff);
		mGrad1.setAlpha(0xff);
		break;
	case BOX_FUTURE:
		mPend.setAlpha(0x32);
		mPenl.setAlpha(0x50);
		mGrad0.setAlpha(0x32);
		mGrad1.setAlpha(0x32);
		break;
	default: //BOX_NONE
		mPend.setAlpha(0xa0);
		mPenl.setAlpha(0xa0);
		mGrad0.setAlpha(0xa0);
		mGrad1.setAlpha(0xa0);
		break;
	}
	update();
}

QRectF EmptyBox::boundingRect() const
{
	return QRect(0, 0, 40, 40);
}

void EmptyBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(Qt::NoPen);

	QRadialGradient grad(QPoint(10, 10), 100);
	grad.setColorAt(0, mGrad0);
	grad.setColorAt(1, mGrad1);
	painter->setBrush(QBrush(grad));
	painter->drawRect(0, 0, 38, 38);

	painter->setPen(QPen(QBrush(mPenl),1));
	painter->drawLine(0, 0, 0, 39);
	painter->drawLine(0, 0, 39, 0);

	painter->setPen(QPen(QBrush(mPend),1));
	painter->drawLine(39, 0, 39, 39);
	painter->drawLine(0, 39, 39, 39);
}
