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

#include "pegbox.h"
#include "QGraphicsEllipseItem"
#include <QPen>
#include <QLinearGradient>

PegBox::PegBox(const QPoint &_position, QGraphicsItem *parent):
	Box(_position, parent),
	m_peg(0)
{

}

int PegBox::getPegColor() const
{
	if (m_peg)
		return m_peg->getColor();

	qDebug("getPegColor called with no peg setted.");
	return -1;
}

bool PegBox::isPegVisible()
{
	return m_peg && m_peg->isVisible();
}

Peg::State PegBox::getPegState() const
{
	if (m_peg) return m_peg->getState();
	return Peg::State::Plain;
}

void PegBox::setState(const Box::State &_state)
{
	m_state = _state;

	if (m_peg) {
		switch (m_state) {
		case Box::State::Past:
			m_peg->setState(Peg::State::Underneath);
			break;
		case Box::State::Current:
			m_peg->setState(Peg::State::Visible);
			break;
		default: // Box::State::None, Box::State::Future
			m_peg->setState(Peg::State::Invisible);
			break;
		}
	}
	update();
}

void PegBox::setPegColor(const int &_color)
{
	if (m_peg)
		m_peg->setColor(_color);
	else
		qDebug("setPegColor called with no peg setted.");
}

void PegBox::setPegState(const Peg::State &_peg_state)
{
	if (m_peg)
		m_peg->setState(_peg_state);
	else
		qDebug("setState called with no peg setted.");

}
