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

#include "peg.h"
#include "pegbox.h"
#include <QPen>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QGraphicsSimpleTextItem>
#include <QFont>

Peg::Peg(const QPoint &position, int color_number, const IndicatorType &indicator_n, QGraphicsItem *parent):
		QGraphicsEllipseItem(2.5, 2.5, 34, 34, parent),
		mPosition(position),
		mBox(0)
{
	mColor = (-1 < color_number &&color_number < 10) ? color_number : 0;

	setPen(Qt::NoPen);

	pressedEffect = new QGraphicsDropShadowEffect;
	pressedEffect->setBlurRadius(10);
	pressedEffect->setXOffset(8);
	setGraphicsEffect(pressedEffect);
	pressedEffect->setEnabled(false);

	auto gloss = new QGraphicsEllipseItem(5, 1.5, 25, 20, this);
	gloss->setRotation(-45);
	gloss->setPos(-5, 19);
	QLinearGradient lgrad(25, 0, 25, 17);
	lgrad.setColorAt(0, QColor(255, 255, 255, 120));
	lgrad.setColorAt(1, QColor(255, 255, 255, 0));
	gloss->setBrush(lgrad);
	gloss->setPen(Qt::NoPen);

	mIndicator = new QGraphicsSimpleTextItem(this);
	QFont font("Monospace", 15, QFont::Bold, false);
	font.setStyleHint(QFont::TypeWriter);
	mIndicator->setFont(font);
	mIndicator->setPos(13.5,7);
	onChangeIndicators(indicator_n);

	setColor(mColor);
	setZValue(2);
	setPos(mPosition);
	setMovable(true);
	setAcceptedMouseButtons(Qt::LeftButton);
	setAcceptDrops(true);
}
//-----------------------------------------------------------------------------

void Peg::setColor(const int &color_number)
{
	mColor = (-1 < color_number && color_number < 10) ? color_number : 0;
	QRadialGradient gradient(7,7, 40, 15, 15);
	gradient.setColorAt(0, PegColors[mColor][0]);
	gradient.setColorAt(1, PegColors[mColor][1]);
	setBrush(gradient);
	onChangeIndicators(mIndicatorType);
}
//-----------------------------------------------------------------------------

void Peg::setMovable(bool enabled)
{
	setFlag(QGraphicsItem::ItemIsMovable, enabled);
	setEnabled(enabled);
	setCursor(enabled ? Qt::OpenHandCursor : Qt::ArrowCursor);
	setZValue(1);
}
//-----------------------------------------------------------------------------

void Peg::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (isEnabled())
	{
		pressedEffect->setEnabled(true);
		setZValue(3);

		if(mBox->getPegState() != PegState::Initial)
			mBox->setPegState(PegState::Draged);

		setCursor(Qt::ClosedHandCursor);
	}
	QGraphicsEllipseItem::mousePressEvent(event);
}
//-----------------------------------------------------------------------------

void Peg::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (isEnabled())
	{
		pressedEffect->setEnabled(false);
		setZValue(2);
		setCursor(Qt::OpenHandCursor);
	}

	QGraphicsEllipseItem::mouseReleaseEvent(event);

	if (mBox->sceneBoundingRect().contains(sceneBoundingRect().center().toPoint()))//	droped on its own box
	{
		if (mBox->getPegState() != PegState::Initial)
			mBox->setPegState(PegState::Filled);
	}
	else //	droped out of its own box,
	{
		if (mBox->getPegState() != PegState::Initial)
			mBox->setPegState(PegState::Empty);
		emit mouseReleasedSignal(sceneBoundingRect().center().toPoint(), mColor);
	}

	setPos(mBox->sceneBoundingRect().topLeft());
}
//-----------------------------------------------------------------------------

void Peg::onChangeIndicators(const IndicatorType &indicator_n)
{
	mIndicatorType = indicator_n;
	mIndicator->setText(OrderedChars[(int)mIndicatorType][mColor]);
}
