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

const QString Peg::OrderedChars[3] = {"ABCDEFGHIJ", "0123456789"};

Peg::Peg(const QPointF &position, int color_number, const bool &show_colors, const bool &show_indicators, const IndicatorType &indicator_n, QGraphicsItem *parent):
	QGraphicsEllipseItem(2.5, 2.5, 34, 34, parent),
	mPosition(position),
	mShowColors(show_colors),
	mShowIndicators(show_indicators),
	mIndicatorType(indicator_n),
	isActive(false)
{
	setPen(Qt::NoPen);
	pressedEffect = new QGraphicsDropShadowEffect;
	pressedEffect->setBlurRadius(10);
	pressedEffect->setXOffset(5);
	setGraphicsEffect(pressedEffect);
	pressedEffect->setEnabled(false);

	mGloss = new QGraphicsEllipseItem(7, 4, 24, 20, this);
	QLinearGradient lgrad(25, 0, 25, 21);
	lgrad.setColorAt(0, QColor(255, 255, 255, 150));
	lgrad.setColorAt(1, QColor(255, 255, 255, 0));
	mGloss->setBrush(lgrad);
	mGloss->setPen(Qt::NoPen);

	mIndicator = new QGraphicsSimpleTextItem(this);
	QFont font("Monospace", 15, QFont::Bold, false);
	font.setStyleHint(QFont::TypeWriter);
	mIndicator->setFont(font);
	mIndicator->setPos(14,8);

	QLinearGradient cgrad(2, 2, 35, 35);
	cgrad.setColorAt(0.0, QColor(80, 80, 80));
	cgrad.setColorAt(1.0, QColor(220, 220, 220));

	mCircle = new QGraphicsEllipseItem(2, 2, 35, 35, this);
	mCircle->setPen(QPen(QBrush(cgrad), 1));

	setColor(color_number);
	setZValue(2);
	setPos(position - QPointF(19.5, 19.5));
	setMovable(true);
	setAcceptDrops(true);
	setState(PegState::Visible);
}
//-----------------------------------------------------------------------------

void Peg::setColor(int color_number)
{
	mColor = (-1 < color_number && color_number < 10) ? color_number : 0;
	if (mShowColors || !mShowIndicators)
	{
		QRadialGradient gradient(20, 0, 60, 20, 0);
		gradient.setColorAt(0, PegColors[mColor][0]);
		gradient.setColorAt(1, PegColors[mColor][1]);
		setBrush(gradient);
	}
	else
	{
		QRadialGradient gradient(20, 0, 60, 20, 0);
		gradient.setColorAt(0, PegColors[3][0]);
		gradient.setColorAt(1, PegColors[3][1]);
		setBrush(gradient);
	}

	mIndicator->setText(OrderedChars[(int)mIndicatorType][mColor]);
	mIndicator->setVisible(mShowIndicators);
}
//-----------------------------------------------------------------------------

void Peg::setMovable(bool enabled)
{
	isActive = enabled;
	setFlag(QGraphicsItem::ItemIsMovable, isActive);
	setCursor(isActive ? Qt::OpenHandCursor : Qt::ArrowCursor);
	setAcceptedMouseButtons(isActive ? Qt::LeftButton : Qt::NoButton);
	setZValue(1+enabled);
}
//-----------------------------------------------------------------------------

void Peg::setState(const PegState &state)
{
	mState = state;
	switch (mState) {
	case PegState::Invisible:
		setVisible(false);
		setMovable(false);
		break;
	case PegState::Underneath:
		setVisible(true);
		setMovable(false);
		break;
	case PegState::Plain:
		setVisible(true);
		setMovable(false);
		mCircle->setVisible(true);
		mIndicator->setVisible(false);
		mGloss->setBrush(Qt::NoBrush);
		setBrush(Qt::NoBrush);
		break;
	default:// PegState::Initial and PegState::Visible
		setVisible(true);
		setMovable(true);
		break;
	}
}
//-----------------------------------------------------------------------------

void Peg::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsEllipseItem::mousePressEvent(event);

	if (event->button() == Qt::LeftButton && isActive)
	{
		mCircle->setVisible(false);
		pressedEffect->setEnabled(true);
		setZValue(3);
		setCursor(Qt::ClosedHandCursor);
	}
}
//-----------------------------------------------------------------------------

void Peg::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsEllipseItem::mouseReleaseEvent(event);

	if (event->button() == Qt::LeftButton && isActive)
	{
		mCircle->setVisible(true);
		pressedEffect->setEnabled(false);
		setZValue(2);
		setCursor(Qt::OpenHandCursor);

		if (!sceneBoundingRect().contains(mPosition))//	droped out of its own box,
		{
			if (mState != PegState::Initial)
				setState(PegState::Invisible);
			emit mouseReleaseSignal(this);
		}
		setPos(mPosition - QPointF(19.5, 19.5));
	}
}
//-----------------------------------------------------------------------------

void Peg::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (isActive)
	{
		QGraphicsEllipseItem::mouseDoubleClickEvent(event);
		if (mState == PegState::Initial)
		{
			emit mouseDoubleClickSignal(this);
			emit mouseReleaseSignal(this);
			setPos(mPosition - QPoint(19.5, 19.5));
		}
		else
			setPos(mPosition - QPoint(19.5, 60));
	}
}
//-----------------------------------------------------------------------------

void Peg::onShowIndicators(const bool &show_colors, const bool &show_indicators, const IndicatorType &indicator_n)
{
	mShowColors = show_colors;
	mShowIndicators = show_indicators;
	mIndicatorType = indicator_n;
	setColor(mColor);
}
