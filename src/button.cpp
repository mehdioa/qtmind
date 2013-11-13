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

#include "button.h"
#include <QGraphicsSceneEvent>
#include <QCursor>
#include <QPainter>

Button::Button(int buttonWidth, QString str)
{
	mWidth = buttonWidth;
	mLabel = str;
	mYOffs = 0;
	mFont = QFont("DejaVu", 11, QFont::Bold, false);
	mFont.setStyleHint(QFont::SansSerif);
	mFont.setStyleStrategy(QFont::PreferAntialias);

	QLinearGradient fillgrad(0, 0, 10, 36);
	fillgrad.setColorAt(0.0, QColor("#f7f8fa"));
	fillgrad.setColorAt(0.5, QColor("#b9babc"));
	mFillOutBrush = QBrush(fillgrad);

	QLinearGradient fillovergrad(0, 0, 10, 36);
	fillovergrad.setColorAt(0.0, QColor("#f7f8fa"));
	fillovergrad.setColorAt(1.0, QColor("#b9babc"));
	mFillOverBrush = QBrush(fillovergrad);

	QLinearGradient framegrad(0, 0, 0, 40);
	framegrad.setColorAt(1.0, QColor(0, 0, 0, 0xa0));
	framegrad.setColorAt(0.0, QColor(0xff, 0xff, 0xff, 0xa0));
	mFrameOutBrush = QBrush(framegrad);

	QLinearGradient frameovergrad(0, 0, 0, 40);
	frameovergrad.setColorAt(0.0, QColor(0, 0, 0, 0xa0));
	frameovergrad.setColorAt(1.0, QColor(0xff, 0xff, 0xff, 0xa0));
	mFrameOverBrush = QBrush(frameovergrad);

	mFrameBrush = &mFrameOutBrush;
	mFillBrush = &mFillOutBrush;

	mRect = QRectF(0, 0, mWidth, 38);
	mRectFill = QRectF(1, 1, mWidth - 2, 36);
	setAcceptedMouseButtons(Qt::LeftButton);
	setAcceptHoverEvents (true);
	setPushable(true);
}
//-----------------------------------------------------------------------------

void Button::setPushable(bool enabled)
{
	setEnabled(enabled);
	setCursor(enabled ? Qt::PointingHandCursor : Qt::ArrowCursor);
}
//-----------------------------------------------------------------------------

void Button::mousePressEvent(QGraphicsSceneMouseEvent *)
{
	mFrameBrush = &mFrameOverBrush;
	mYOffs = 1;
	update(mRect);
}
//-----------------------------------------------------------------------------

void Button::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	mFrameBrush = &mFrameOutBrush;
	mYOffs = 0;
	update(mRect);
	if (boundingRect().contains(event->pos()))
		emit buttonPressed();
}
//-----------------------------------------------------------------------------

void Button::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
	mFillBrush = &mFillOverBrush;
	update(mRect);
}
//-----------------------------------------------------------------------------

void Button::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
	mFillBrush = &mFillOutBrush;
	update(mRect);
}
//-----------------------------------------------------------------------------

void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(Qt::NoPen);
	painter->setBrush(*mFrameBrush);
	painter->drawRoundedRect(mRect, 20, 20);

	painter->setBrush(*mFillBrush);
	painter->drawRoundedRect(mRectFill, 18, 18);

	if (mLabel != "")
	{
		painter->setRenderHint(QPainter::TextAntialiasing, true);
		painter->setPen(QPen(QColor("#303133")));
		painter->setFont(mFont);
		painter->drawText(mRectFill.adjusted(0, mYOffs, 0, mYOffs), Qt::AlignCenter, mLabel);
	}
}
//-----------------------------------------------------------------------------

QRectF Button::boundingRect() const
{
	return mRect.adjusted(-0.5, -0.5, 1, 1);
}
