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

const int PinBox::PinPositions[Rules::MAX_SLOT_NUMBER][Rules::MAX_SLOT_NUMBER][2] = /**< TODO */
{
	{{4, 13}, {22, 13}, {0, 0}, {0, 0}, {0, 0}},
	{{4, 6}, {22, 6}, {13, 22}, {0, 0}, {0, 0}},
	{{4, 4}, {22, 4}, {4, 22}, {22, 22}, {0, 0}},
	{{2, 2}, {24, 2}, {13, 13}, {2, 24}, {24, 24}}
};

PinBox::PinBox(const int &pin_number, const QPoint &m_position, QGraphicsItem *parent):
	Box(m_position, parent)
{
	for(int i = 0; i < pin_number; ++i) {
		auto pin = new Pin(this);
		pins.append(pin);
		pin->setPos(PinPositions[pin_number-2][i][0], PinPositions[pin_number-2][i][1]);
	}
	setState(Box::State::Future);
}

int PinBox::getValue() const
{
	int blacks = 0;
	int whites = 0;
	foreach(Pin *pin, pins) {
		switch (pin->getColor()) {
		case Pin::Color::White:
			++whites;
			break;
		case Pin::Color::Black:
			++blacks;
			break;
		default:
			break;
		}
	}
	return (blacks + whites)*(blacks + whites + 1) / 2 + blacks;
}

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
	for (int i = 0; i < color_n; ++i)
		total += qMin(c[i],g[i]);

	for(int i = 0; i < blacks; ++i)
		pins.at(i)->setColor(Pin::Color::Black);

	for(int i = blacks; i < total; ++i)
		pins.at(i)->setColor(Pin::Color::White);
}

void PinBox::setState(const Box::State &m_state)
{
	state = m_state;
	bool pin_activity = (state == Box::State::None);
	foreach(Pin *pin, pins)
		pin->setActivity(pin_activity);
	update();
}

