/***********************************************************************
 *
 * Copyright (C) 2013 Mehdi Omidal <mehdioa@gmail.com>
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

const int PinBox::pin_positions[5][5][2] = {
	{{5, 14}, {22, 14}, {0, 0}, {0, 0}, {0, 0}},
	{{4, 7}, {22, 7}, {14, 21}, {0, 0}, {0, 0}},
	{{6, 6}, {22, 6}, {6, 22}, {22, 22}, {0, 0}},
	{{4, 4}, {24, 4}, {14, 14}, {4, 24}, {24, 24}}
};
PinBox::PinBox(const int& pin_number, const QPoint& position, QGraphicsItem* parent):
	EmptyBox(position, parent)
{
	for(int i = 0; i < pin_number; ++i)
	{
		auto pin = new Pin(0, this);
		pins.append(pin);
		pin->setPos(pin_positions[pin_number-2][i][0], pin_positions[pin_number-2][i][1]);
	}
	setState(BOX_FUTURE);
	setAcceptedMouseButtons(Qt::LeftButton);
}

int PinBox::getValue(int &blacks, int &whites) const
{
	blacks = 0;
	whites = 0;
	foreach(Pin* pin, pins)
	{
		switch (pin->getColor()) {
		case 1:
			blacks++;
			break;
		case -1:
			whites++;
			break;
		default:
			break;
		}
	}
	return (blacks + whites)*(blacks + whites + 1) / 2 + blacks;
}

int PinBox::getValue() const
{
	int blacks;
	int whites;
	return getValue(blacks, whites);
}


void PinBox::setPins(const int &blacks, const int &whites)
{
	for(int i = 0; i < blacks; ++i)
		pins.at(i)->setColor(1);
	for(int i = 0; i < whites; ++i)
		pins.at(blacks + i)->setColor(-1);
}

void PinBox::setPins(const QString &codeA, const QString &codeB, const int &peg, const int &color)
{
	int c[color];
	int g[color];
	std::fill(c, c+color, 0);
	std::fill(g, g+color, 0);

	int blacks = 0;
	QString NUMS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (int i = 0; i < peg; ++i) {
		blacks += (codeA[i] == codeB[i]);
		++c[NUMS.indexOf(codeA[i])];
		++g[NUMS.indexOf(codeB[i])];
	}

	int total = 0;		//	blacks + whites
	for (int i = 0; i < color; ++i){
		total += std::min(c[i],g[i]);
	}

	setPins(blacks, total-blacks);
}

void PinBox::setState(const BOX_STATE &state)
{
	m_state = state;
	setHighlight();

	switch (m_state) {
	case BOX_PAST: // used for boxes that are done. No interaction allowed
		setEnabled(false);
		setCursor(Qt::ArrowCursor);
		foreach (Pin* pin, pins)
			pin->setMouseEventHandling(MOUSE_IGNORE);
		break;
	case BOX_CURRENT://	used for Master mode that the user press the box when he/she is satisfied with their guess
		setEnabled(true);
		setCursor(Qt::PointingHandCursor);
		foreach (Pin* pin, pins)
			pin->setMouseEventHandling(MOUSE_TOBOX);
		break;
	case BOX_FUTURE: //BOX_FUTURE: used for boxes that they are done or it is not their time yet. no interaction allowed
		setEnabled(false);
		setCursor(Qt::ArrowCursor);
		foreach (Pin* pin, pins)
			pin->setMouseEventHandling(MOUSE_IGNORE);
		break;

	default: //BOX_NONE: used for entering keys in breaker mode, just keys are active
		setEnabled(false);
		setCursor(Qt::ArrowCursor);
		foreach (Pin* pin, pins)
			pin->setMouseEventHandling(MOUSE_ACCEPT);
		break;
	}
}

void PinBox::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	emit pinBoxPushed();
}
