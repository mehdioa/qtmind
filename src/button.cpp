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

#include "button.h"
#include "board.h"
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneEvent>
#include <QCursor>
#include <QPainter>

Button::Button(const int &buttonWidth, const QString &str, QGraphicsItem *parent):
	QGraphicsObject(parent)
{
    mWidth = buttonWidth;
    mLabel = str;

    mFont = QFont(Board::instance()->fontName(), Board::instance()->fontSize() - 1, QFont::Bold, false);
    mFont.setStyleHint(QFont::SansSerif);
    mFont.setStyleStrategy(QFont::PreferAntialias);

    mPressedEffect = new QGraphicsDropShadowEffect;
    mPressedEffect->setOffset(0, 2);
    setGraphicsEffect(mPressedEffect);
    mPressedEffect->setEnabled(true);

	setAcceptedMouseButtons(Qt::LeftButton);
	setAcceptHoverEvents (true);
	setEnabled(true);
	setCursor(Qt::PointingHandCursor);
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    mPressedEffect->setOffset(0, 1);
	moveBy(0, 1);
}

void Button::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	moveBy(0, -1);
    mPressedEffect->setOffset(0, 2);
	if (boundingRect().contains(event->pos()))
		emit buttonPressed();
}

void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(Qt::NoPen);

	QLinearGradient framegrad(0, 0, 0, 40);
	framegrad.setColorAt(0.0, QColor(248, 248, 248));
	framegrad.setColorAt(0.6, QColor(184, 184, 184));
	framegrad.setColorAt(1, QColor(212, 212, 212));
	painter->setBrush(QBrush(framegrad));
	painter->drawRoundedRect(boundingRect(), 20, 20);

    if (mLabel != "") {
		painter->setRenderHint(QPainter::TextAntialiasing, true);
		painter->setPen(QPen(QColor("#303030")));
        painter->setFont(mFont);
        painter->drawText(boundingRect(), Qt::AlignCenter, mLabel);
	}
}

QRectF Button::boundingRect() const
{
    return QRectF(1, 0, mWidth, 36);
}
