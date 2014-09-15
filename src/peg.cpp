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
#include "board.h"
#include "appinfo.h"
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QPainter>
#include <QTapGesture>
#include <QGestureEvent>

const QColor Peg::s_pegColors[MAX_COLOR_NUMBER][2] = { /**< TODO */
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

const QFont Peg::s_font = Peg::setFont(); /**< TODO */

Peg::Peg(const QPointF &_position, const int &_color, QGraphicsItem *parent):
	QGraphicsObject(parent),
	m_position(_position),
	m_movable(false)
{
	m_pressedEffect = new QGraphicsDropShadowEffect;
	m_pressedEffect->setBlurRadius(10);
	m_pressedEffect->setXOffset(2);
	setGraphicsEffect(m_pressedEffect);
	m_pressedEffect->setEnabled(false);

	QLinearGradient cgrad(2, 2, 2, 35);
	cgrad.setColorAt(0.0, QColor(50, 50, 50));
	cgrad.setColorAt(1.0, QColor(240, 240, 240));

	m_circle = new QGraphicsEllipseItem(2, 2, 35, 35, this);
	m_circle->setPen(QPen(QBrush(cgrad), 1));

	setColor(_color);
	setZValue(2);
	setPos(m_position - QPointF(19.5, 19.5));
	setMovable(true);
	setAcceptDrops(true);
	setState(State::Visible);
	grabGesture(Qt::TapGesture);
}

void Peg::setColor(const int &_color)
{
	m_color = (-1 < _color && _color < 10) ? _color : 0;
	update();
}

void Peg::setMovable(bool b)
{
	m_movable = b;
	setFlag(QGraphicsItem::ItemIsMovable, m_movable);
	setCursor(m_movable ? Qt::OpenHandCursor : Qt::ArrowCursor);
	setAcceptedMouseButtons(m_movable ? Qt::LeftButton : Qt::NoButton);
	setZValue(1+b);
	b ? grabGesture(Qt::TapGesture) : ungrabGesture(Qt::TapGesture);
}

void Peg::setState(const State &_state)
{
	m_state = _state;
	switch (m_state) {
	case State::Invisible:
		setVisible(false);
		setMovable(false);
		break;
	case State::Underneath:
		setVisible(true);
		setMovable(false);
		break;
	case State::Plain:
		setVisible(true);
		setMovable(false);
		m_circle->setVisible(true);
		break;
	default:// State::Initial and State::Visible
		setVisible(true);
		setMovable(true);
		break;
	}
}

void Peg::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);

	if (event->button() == Qt::LeftButton && m_movable) {
		m_circle->setVisible(false);
		m_pressedEffect->setEnabled(true);
		setZValue(3);
		setCursor(Qt::ClosedHandCursor);
	}
}

void Peg::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseReleaseEvent(event);

	if (event->button() == Qt::LeftButton && m_movable) {
		m_circle->setVisible(true);
		m_pressedEffect->setEnabled(false);
		setZValue(2);
		setCursor(Qt::OpenHandCursor);

		//	dropped out of its own box,
		if (!sceneBoundingRect().contains(m_position)) {
			if (m_state != State::Initial)
				setState(State::Invisible);
			emit mouseReleaseSignal(this);
		}
		setPos(m_position - QPointF(19.5, 19.5));
	}
}

void Peg::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_movable)
	{
		QGraphicsItem::mouseDoubleClickEvent(event);
		if (m_state == State::Initial){
			emit mouseDoubleClickSignal(this);
		} else {
			setPos(m_position - QPoint(19, 60));
		}
	}
}

bool Peg::event(QEvent *event)
{
	if (event->type() == QEvent::Gesture && isMovable()) {
		return gestureEvent(static_cast<QGestureEvent*>(event));
	}
	return QGraphicsObject::event(event);
}

bool Peg::gestureEvent(QGestureEvent *event)
{
	if ( QGesture *tap_gesture = event->gesture( Qt::TapGesture ) ) {
	  tapGestureTriggered(static_cast<QTapGesture *>( tap_gesture ) );
	}
	return true;
}

void Peg::tapGestureTriggered(QTapGesture *gesture)
{
	if ( gesture->state() == Qt::GestureFinished) {
		if (m_state == State::Initial) {
			emit mouseDoubleClickSignal(this);
		} else {
			setPos(m_position - QPoint(19, 60));
		}
	}
}

void Peg::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setPen(Qt::NoPen);
	int virtual_color = Board::instance()->forceColor() ? m_color : 3;

	if (m_state != State::Plain) {
		QLinearGradient gradient(2.5, 2.5, 2.5, 35);
		gradient.setColorAt(0, s_pegColors[virtual_color][0]);
		gradient.setColorAt(1, s_pegColors[virtual_color][1]);
		painter->setBrush(gradient);
		painter->drawEllipse(2, 2, 35, 35);

		QLinearGradient lgrad(0, 0, 0, 20);
		lgrad.setColorAt(0, QColor(255, 255, 255, 180));
		lgrad.setColorAt(1, QColor(255, 255, 255, 0));
		painter->setPen(Qt::NoPen);
		painter->setBrush(lgrad);
		painter->drawEllipse(6, 4, 27, 20);
	} else {
		painter->setBrush(Qt::NoBrush);
		painter->drawEllipse(2, 2, 34, 34);
		painter->drawEllipse(7, 4, 24, 20);
	}

	if (Board::instance()->m_showIndicators && m_state != State::Plain) {

		painter->setRenderHint(QPainter::TextAntialiasing, true);
		painter->setPen(QPen(Qt::black));
		painter->setFont(s_font);
		painter->drawText(boundingRect(), Qt::AlignCenter, QString((QChar)((int)Board::instance()->m_indicator+m_color)));
	}
}

QRectF Peg::boundingRect() const
{
	return QRectF(2.5, 2.5, 35, 35);
}

QFont Peg::setFont()
{
	QFont m_font = QFont("Monospace", 15, QFont::Bold, false);
	m_font.setStyleHint(QFont::Monospace);
	m_font.setStyleStrategy(QFont::PreferAntialias);
	return m_font;
}

void Peg::onShowIndicators()
{
	update();
}
