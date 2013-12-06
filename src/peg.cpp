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

const QColor Peg::PegColors[MAX_COLOR_NUMBER][2] = {
	{QColor("#FFFF80"), QColor("#C05800")},
	{QColor("#FF3300"), QColor("#400040")},
	{QColor("#33CCFF"), QColor("#000080")},
	{QColor("#ffffff"), QColor("#666666")},
	{QColor("#808080"), QColor("#000000")},
	{QColor("#66FF33"), QColor("#385009")},
	{QColor("#FF9900"), QColor("#A82A00")},
	{QColor("#BA88FF"), QColor("#38005D")},
	{QColor("#00FFFF"), QColor("#004040")},
	{QColor("#FFC0FF"), QColor("#800080")}
};

const QString Peg::OrderedChars[3] = {"          ", "ABCDEFGHIJ", "0123456789"};

Peg::Peg(const QPoint &position, int color_number, const IndicatorType &indicator_n, QGraphicsItem *parent):
		QGraphicsEllipseItem(2.5, 2.5, 34, 34, parent),
		mPosition(position),
		mBox(0),
		isActive(false)
{
	mColor = (-1 < color_number &&color_number < 10) ? color_number : 0;

	setPen(Qt::NoPen);

	pressedEffect = new QGraphicsDropShadowEffect;
	pressedEffect->setBlurRadius(10);
	pressedEffect->setXOffset(5);
	setGraphicsEffect(pressedEffect);
	pressedEffect->setEnabled(false);

	auto gloss = new QGraphicsEllipseItem(7, 4, 24, 20, this);
	QLinearGradient lgrad(25, 0, 25, 21);
	lgrad.setColorAt(0, QColor(255, 255, 255, 150));
	lgrad.setColorAt(1, QColor(255, 255, 255, 0));
	gloss->setBrush(lgrad);
	gloss->setPen(Qt::NoPen);

	mIndicator = new QGraphicsSimpleTextItem(this);
	QFont font("Monospace", 15, QFont::Bold, false);
	font.setStyleHint(QFont::TypeWriter);
	mIndicator->setFont(font);
	mIndicator->setPos(13.5,7);
	onIndicatorChanged(indicator_n);

	setColor(mColor);
	setZValue(2);
	setPos(mPosition);
	setMovable(true);
	setAcceptDrops(true);
}
//-----------------------------------------------------------------------------

void Peg::setColor(const int &color_number)
{
	mColor = (-1 < color_number && color_number < 10) ? color_number : 0;
	QRadialGradient gradient(20, 0, 60, 20, 0);
	gradient.setColorAt(0, PegColors[mColor][0]);
	gradient.setColorAt(1, PegColors[mColor][1]);
	setBrush(gradient);
	onIndicatorChanged(mIndicatorType);
}
//-----------------------------------------------------------------------------

void Peg::setMovable(bool enabled)
{
	isActive = enabled;
	setFlag(QGraphicsItem::ItemIsMovable, isActive);
	setCursor(isActive ? Qt::OpenHandCursor : Qt::ArrowCursor);
	setAcceptedMouseButtons(isActive ? Qt::LeftButton : Qt::NoButton);
	setZValue(1);
}
//-----------------------------------------------------------------------------

void Peg::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsEllipseItem::mousePressEvent(event);

	if (event->button() == Qt::LeftButton && isActive)
	{
		pressedEffect->setEnabled(true);
		setZValue(3);

		if(mBox->getPegState() != PegState::Initial)
			mBox->setPegState(PegState::Draged);

		setCursor(Qt::ClosedHandCursor);
	}
}
//-----------------------------------------------------------------------------

void Peg::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsEllipseItem::mouseReleaseEvent(event);

	if (event->button() == Qt::LeftButton && isActive)
	{
		pressedEffect->setEnabled(false);
		setZValue(2);
		setCursor(Qt::OpenHandCursor);

		if (mBox->sceneBoundingRect().contains(sceneBoundingRect().center().toPoint()))//	droped on its own box
		{
			if (mBox->getPegState() != PegState::Initial)
				mBox->setPegState(PegState::Visible);
		}
		else //	droped out of its own box,
		{
			if (mBox->getPegState() != PegState::Initial)
				mBox->setPegState(PegState::Invisible);
			emit mouseReleaseSignal(sceneBoundingRect().center().toPoint(), mColor);
		}

		setPos(mBox->sceneBoundingRect().topLeft());
	}

}
//-----------------------------------------------------------------------------

void Peg::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (isActive)
	{
		QGraphicsEllipseItem::mouseDoubleClickEvent(event);
		if (mBox && mBox->getPegState() == PegState::Initial)
		{
			emit mouseDoubleClickSignal(this);
			emit mouseReleaseSignal(sceneBoundingRect().center().toPoint(), mColor);
			setPos(mBox->sceneBoundingRect().topLeft().toPoint());
		}
	}
}

//-----------------------------------------------------------------------------

void Peg::onIndicatorChanged(const IndicatorType &indicator_n)
{
	mIndicatorType = indicator_n;
	mIndicator->setText(OrderedChars[(int)mIndicatorType][mColor]);
}
