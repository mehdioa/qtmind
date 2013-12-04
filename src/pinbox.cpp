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

int PinBox::getValue() const
{
	int blacks = 0;
	int whites = 0;
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

void PinBox::setPins(const QString &codeA, const QString &codeB, const int &color_n)
{
	int c[color_n];
	int g[color_n];
	std::fill(c, c+color_n, 0);
	std::fill(g, g+color_n, 0);

	int blacks = 0;
	for (int i = 0; i < pins.size(); ++i) {
		if (codeA[i] == codeB[i])
			++blacks;
		++c[codeA[i].digitValue()];
		++g[codeB[i].digitValue()];
	}

	int total = 0;		//	blacks + whites
	for (int i = 0; i < color_n; ++i){
		total += qMin(c[i],g[i]);
	}

	for(int i = 0; i < blacks; ++i)
		pins.at(i)->setColor(1);

	for(int i = blacks; i < total; ++i)
		pins.at(i)->setColor(-1);
}
//-----------------------------------------------------------------------------

void PinBox::setBoxState(const BoxState &state)
{
	mBoxState = state;
	switch (mBoxState) {
	case BoxState::Current://	used for Master mode that the user press the box when he/she is satisfied with their guess
		isActive = true;
		setAcceptedMouseButtons(Qt::LeftButton);
		setCursor(Qt::PointingHandCursor);
		foreach (Pin *pin, pins)
		{
			pin->setActivity(false);
			pin->setAcceptedMouseButtons(Qt::NoButton);
			pin->setCursor(Qt::PointingHandCursor);
		}
		break;
	case BoxState::None: //BoxState::None: used for entering pins in breaker mode, just pins are active
		isActive = false;
		setCursor(Qt::ArrowCursor);
		setAcceptedMouseButtons(Qt::NoButton);
		foreach (Pin *pin, pins)
		{
			pin->setActivity(true);
			pin->setAcceptedMouseButtons(Qt::LeftButton);
			pin->setCursor(Qt::PointingHandCursor);
		}
		break;
	default: //BoxState::FUTURE and BoxState::Past, no interaction is allowed
		isActive = false;
		setCursor(Qt::ArrowCursor);
		setAcceptedMouseButtons(Qt::NoButton);
		foreach (Pin *pin, pins)
		{
			pin->setActivity(false);
			pin->setAcceptedMouseButtons(Qt::NoButton);
			pin->setCursor(Qt::ArrowCursor);
		}
		break;
	}
	update();
}
//-----------------------------------------------------------------------------

void PinBox::mousePressEvent(QGraphicsSceneMouseEvent *)
{
	if (isActive)
		emit pinBoxPressSignal();
}
