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
#include "indicator.h"
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QPainter>
#include <QTapGesture>
#include <QGestureEvent>

const QColor Peg::PegColors[MAX_COLOR_NUMBER][2] = {
	{QColor("#FFFF80"), QColor("#E47A00")},
	{QColor("#FF3300"), QColor("#AF0707")},
	{QColor("#33CCFF"), QColor("#031CFF")},
	{QColor("#ffffff"), QColor("#999999")},
	{QColor("#808080"), QColor("#000000")},
	{QColor("#66FF33"), QColor("#597F0E")},
	{QColor("#FF9900"), QColor("#DB5000")},
	{QColor("#BA88FF"), QColor("#6300A5")},
	{QColor("#00FFFF"), QColor("#007A7A")},
	{QColor("#FFC0FF"), QColor("#AB00AB")}
};

const QString Peg::OrderedChars[3] = {"ABCDEFGHIJ", "0123456789"};

const QFont Peg::font = Peg::setFont();

Peg::Peg(const QPointF &m_position, const int &color_number, Indicator *indicator_s, QGraphicsItem *parent):
	QGraphicsObject(parent),
	position(m_position),
	indicator(indicator_s),
	isActive(false)
{
	pressedEffect = new QGraphicsDropShadowEffect;
	pressedEffect->setBlurRadius(10);
	pressedEffect->setXOffset(2);
	setGraphicsEffect(pressedEffect);
	pressedEffect->setEnabled(false);

	QLinearGradient cgrad(2, 2, 2, 35);
	cgrad.setColorAt(0.0, QColor(50, 50, 50));
	cgrad.setColorAt(1.0, QColor(240, 240, 240));

	circle = new QGraphicsEllipseItem(2, 2, 35, 35, this);
	circle->setPen(QPen(QBrush(cgrad), 1));

	setColor(color_number);
	setZValue(2);
	setPos(m_position - QPointF(19.5, 19.5));
	setMovable(true);
	setAcceptDrops(true);
	setState(PegState::Visible);
	grabGesture(Qt::TapGesture);
}
//-----------------------------------------------------------------------------

void Peg::setColor(int color_number)
{
	color = (-1 < color_number && color_number < 10) ? color_number : 0;
	update();
}
//-----------------------------------------------------------------------------

void Peg::setMovable(bool enabled)
{
	isActive = enabled;
	setFlag(QGraphicsItem::ItemIsMovable, isActive);
	setCursor(isActive ? Qt::OpenHandCursor : Qt::ArrowCursor);
	setAcceptedMouseButtons(isActive ? Qt::LeftButton : Qt::NoButton);
	setZValue(1+enabled);
	enabled ? grabGesture(Qt::TapGesture) : ungrabGesture(Qt::TapGesture);
}
//-----------------------------------------------------------------------------

void Peg::setState(const PegState &m_state)
{
	pegState = m_state;
	switch (pegState) {
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
		circle->setVisible(true);
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
	QGraphicsItem::mousePressEvent(event);

	if (event->button() == Qt::LeftButton && isActive) {
		circle->setVisible(false);
		pressedEffect->setEnabled(true);
		setZValue(3);
		setCursor(Qt::ClosedHandCursor);
	}
}
//-----------------------------------------------------------------------------

void Peg::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseReleaseEvent(event);

	if (event->button() == Qt::LeftButton && isActive) {
		circle->setVisible(true);
		pressedEffect->setEnabled(false);
		setZValue(2);
		setCursor(Qt::OpenHandCursor);

		//	droped out of its own box,
		if (!sceneBoundingRect().contains(position)) {
			if (pegState != PegState::Initial)
				setState(PegState::Invisible);
			emit mouseReleaseSignal(this);
		}
		setPos(position - QPointF(19.5, 19.5));
	}
}
//-----------------------------------------------------------------------------

void Peg::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (isActive)
	{
		QGraphicsItem::mouseDoubleClickEvent(event);
		if (pegState == PegState::Initial){
			emit mouseDoubleClickSignal(this);
		} else {
			setPos(position - QPoint(19.5, 60));
		}
	}
}
//-----------------------------------------------------------------------------

bool Peg::event(QEvent *event)
{
	if (event->type() == QEvent::Gesture && isMovable()) {
		return gestureEvent(static_cast<QGestureEvent*>(event));
	}
	return QGraphicsObject::event(event);
}
//-----------------------------------------------------------------------------

bool Peg::gestureEvent(QGestureEvent *event)
{
	if ( QGesture *tap_gesture = event->gesture( Qt::TapGesture ) ) {
	  tapGestureTriggered(static_cast<QTapGesture *>( tap_gesture ) );
	}
	return true;
}
//-----------------------------------------------------------------------------

void Peg::tapGestureTriggered(QTapGesture *gesture)
{
	if ( gesture->state() == Qt::GestureFinished) {
		if (pegState == PegState::Initial) {
			emit mouseDoubleClickSignal(this);
		} else {
			setPos(position - QPoint(19.5, 60));
		}
	}
}
//-----------------------------------------------------------------------------

void Peg::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(Qt::NoPen);
	int virtual_color = indicator->forceColor() ? color : 3;

	if (pegState != PegState::Plain) {
		QLinearGradient gradient(2.5, 2.5, 2.5, 35);
		gradient.setColorAt(0, PegColors[virtual_color][0]);
		gradient.setColorAt(1, PegColors[virtual_color][1]);
		painter->setBrush(gradient);
		painter->drawEllipse(2.5, 2.5, 35, 35);

		QLinearGradient lgrad(0, 0, 0, 20);
		lgrad.setColorAt(0, QColor(255, 255, 255, 180));
		lgrad.setColorAt(1, QColor(255, 255, 255, 0));
		painter->setPen(Qt::NoPen);
		painter->setBrush(lgrad);
		painter->drawEllipse(6, 4, 27, 20);
	} else {
		painter->setBrush(Qt::NoBrush);
		painter->drawEllipse(2.5, 2.5, 34, 34);
		painter->drawEllipse(7, 4, 24, 20);
	}

	if (indicator->showIndicators && pegState != PegState::Plain) {

		painter->setRenderHint(QPainter::TextAntialiasing, true);
		painter->setPen(QPen(Qt::black));
		painter->setFont(font);
		painter->drawText(boundingRect(), Qt::AlignCenter, OrderedChars[(int)indicator->indicatorType][color]);
	}
}
//-----------------------------------------------------------------------------

QRectF Peg::boundingRect() const
{
	return QRectF(2.5, 2.5, 35, 35);
}
//-----------------------------------------------------------------------------

QFont Peg::setFont()
{
	QFont m_font = QFont("Monospace", 15, QFont::Bold, false);
	m_font.setStyleHint(QFont::Monospace);
	m_font.setStyleStrategy(QFont::PreferAntialias);
	return m_font;
}
//-----------------------------------------------------------------------------

void Peg::onShowIndicators()
{
	update();
}
