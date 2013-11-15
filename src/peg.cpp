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

const QString Peg::color_rgb[10][2] = {
		{"#FFFF80", "#C05800"}, {"#FF3300", "#400040"},
		{"#33CCFF", "#000080"}, {"#808080", "#000000"},
		{"#FFFFFF", "#797979"},{"#FF9900", "#A82A00"},
		{"#66FF33", "#385009"},{"#BA88FF", "#38005D"},
		{"#00FFFF", "#004040"}, {"#FFC0FF", "#800080"}
  };
const QString Peg::ordered_chars[3] = {"          ", "ABCDEFGHIJ", "0123456789"};

Peg::Peg(const QPoint& position, int color_number, const int &indicator_n, QGraphicsItem *parent):
		QGraphicsEllipseItem(2.5, 2.5, 34, 34, parent),
		mPosition(position),
		mBox(0)
{
	mColor = (-1 < color_number && color_number < 10) ? color_number : 0;

	setPen(Qt::NoPen);

	pressedEffect = new QGraphicsDropShadowEffect;
	pressedEffect->setBlurRadius(10);
	pressedEffect->setXOffset(8);
	setGraphicsEffect(pressedEffect);
	pressedEffect->setEnabled(false);

	auto gloss = new QGraphicsEllipseItem(5, 1.5, 25, 20, this);
	gloss->rotate(-45);
	gloss->setPos(-5, 19);
	QLinearGradient lgrad(25, 0, 25, 17);
	lgrad.setColorAt(0, QColor(0xff, 0xff, 0xff, 120));
	lgrad.setColorAt(1, QColor(0xff, 0xff, 0xff, 0x00));
	gloss->setBrush(lgrad);
	gloss->setPen(Qt::NoPen);

	mIndicator = new QGraphicsSimpleTextItem(this);
	mIndicator->setFont( QFont("DejaVu Sans Mono", 12, QFont::Bold, false));
	mIndicator->setPos(12,6);
	mIndicator->setScale(1.5);
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
	QRadialGradient gradient(20,20, 40, 10, 10);
	gradient.setColorAt(0, QColor(color_rgb[mColor][0]));
	gradient.setColorAt(1, QColor(color_rgb[mColor][1]));
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
	if (isEnabled()){
		pressedEffect->setEnabled(true);
		setZValue(3);
		if(mBox->getPegState() != PEG_INITIAL) mBox->setPegState(PEG_DRAGED);
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

	switch (!mBox->sceneBoundingRect().contains(sceneBoundingRect().center().toPoint())) {
	case 0: //	droped on its own box, no need to emit signal and set state to PEG_FILLED
		if (mBox->getPegState() != PEG_INITIAL)
			mBox->setPegState(PEG_FILLED);
		break;
	default: //	droped out of its own box,
		if (mBox->getPegState() != PEG_INITIAL)
			mBox->setPegState(PEG_EMPTY);
		emit mouseReleasedSignal(sceneBoundingRect().center().toPoint(), mColor);
		break;
	}

	setPos(mBox->sceneBoundingRect().topLeft());
}
//-----------------------------------------------------------------------------

void Peg::onChangeIndicators(const int indicator_n)
{
	mIndicatorType = (INDICATOR_TYPE) indicator_n;
	mIndicator->setText(ordered_chars[mIndicatorType][mColor]);
}
