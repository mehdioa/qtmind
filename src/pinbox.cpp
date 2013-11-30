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

#include "pinbox.h"
#include "pin.h"
#include <QPainter>
#include <QPen>
#include <QCursor>

PinBox::PinBox(const int &pin_number, const QPoint &position, QGraphicsItem *parent):
	EmptyBox(position, parent),
	isActive(false)
{
	for(int i = 0; i < pin_number; ++i)
	{
		auto pin = new Pin(0, this);
		pins.append(pin);
		pin->setPos(PinPositions[pin_number-2][i][0], PinPositions[pin_number-2][i][1]);
	}
	setBoxState(BoxState::Future);
}
//-----------------------------------------------------------------------------

int PinBox::getValue(int &blacks, int &whites) const
{
	blacks = 0;
	whites = 0;
	foreach(Pin *pin, pins)
	{
		switch (pin->getColor()) {
		case 1:
			++blacks;
			break;
		case -1:
			++whites;
			break;
		default:
			break;
		}
	}
	return (blacks + whites)*(blacks + whites + 1) / 2 + blacks;
}
//-----------------------------------------------------------------------------

int PinBox::getValue() const
{
	if (mBoxState != BoxState::None)
		return -1;

	int blacks;
	int whites;
	return getValue(blacks, whites);
}
//-----------------------------------------------------------------------------

void PinBox::setPins(const int &blacks, const int &whites)
{
	for(int i = 0; i < blacks; ++i)
		pins.at(i)->setColor(1);

	for(int i = 0; i < whites; ++i)
		pins.at(blacks + i)->setColor(-1);
}
//-----------------------------------------------------------------------------

void PinBox::setPins(const QString &codeA, const QString &codeB, const int &peg, const int &color)
{
	int c[color];
	int g[color];
	std::fill(c, c+color, 0);
	std::fill(g, g+color, 0);

	int blacks = 0;
	for (int i = 0; i < peg; ++i) {
		blacks += (codeA[i] == codeB[i]);
		++c[codeA[i].digitValue()];
		++g[codeB[i].digitValue()];
	}

	int total = 0;		//	blacks + whites
	for (int i = 0; i < color; ++i){
		total += qMin(c[i],g[i]);
	}

	setPins(blacks, total-blacks);
}
//-----------------------------------------------------------------------------

void PinBox::setBoxState(const BoxState &state)
{
	mBoxState = state;
	switch (mBoxState) {
	case BoxState::Past: // used for boxes that are done. No interaction allowed
		isActive = false;
		setCursor(Qt::ArrowCursor);
		foreach (Pin *pin, pins)
			pin->setMouseEventHandling(PinMouse::Ignore);
		break;
	case BoxState::Current://	used for Master mode that the user press the box when he/she is satisfied with their guess
		isActive = true;
		setCursor(Qt::PointingHandCursor);
		foreach (Pin *pin, pins)
			pin->setMouseEventHandling(PinMouse::Pass);
		break;
	case BoxState::Future: //BOX_FUTURE: used for boxes that are done or it is not their time yet. no interaction allowed
		isActive = false;
		setCursor(Qt::ArrowCursor);
		foreach (Pin *pin, pins)
			pin->setMouseEventHandling(PinMouse::Ignore);
		break;

	default: //BoxState::None: used for entering keys in breaker mode, just keys are active
		isActive = false;
		setCursor(Qt::ArrowCursor);
		foreach (Pin *pin, pins)
			pin->setMouseEventHandling(PinMouse::Accept);
		break;
	}
	update();
}
//-----------------------------------------------------------------------------

void PinBox::mousePressEvent(QGraphicsSceneMouseEvent *)
{
	if (isActive)
		emit pinBoxPressed();
}
