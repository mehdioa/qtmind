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

PegBox::PegBox(const QPoint &m_position, QGraphicsItem *parent):
	Box(m_position, parent),
	peg(0)
{

}

int PegBox::getPegColor() const
{
	if (peg)
		return peg->getColor();

	qDebug("getPegColor called with no peg setted.");
	return -1;
}

bool PegBox::isPegVisible()
{
	return peg && peg->isVisible();
}

Peg::State PegBox::getPegState() const
{
	if (peg) return peg->getState();
	return Peg::State::Plain;
}

void PegBox::setState(const Box::State &m_state)
{
	state = m_state;

	if (peg) {
		switch (state) {
		case Box::State::Past:
			peg->setState(Peg::State::Underneath);
			break;
		case Box::State::Current:
			peg->setState(Peg::State::Visible);
			break;
		default: // Box::State::None, Box::State::Future
			peg->setState(Peg::State::Invisible);
			break;
		}
	}
	update();
}

void PegBox::setPegColor(const int &color_number)
{
	if (peg)
		peg->setColor(color_number);
	else
		qDebug("setPegColor called with no peg setted.");
}

void PegBox::setPegState(const Peg::State &peg_state)
{
	if (peg)
		peg->setState(peg_state);
	else
		qDebug("setState called with no peg setted.");

}
