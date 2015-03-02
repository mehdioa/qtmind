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

const int PinBox::sPinPositions[MAX_SLOT_NUMBER][MAX_SLOT_NUMBER][2] = /**< TODO */
{
	{{4, 13}, {22, 13}, {0, 0}, {0, 0}, {0, 0}},
	{{4, 6}, {22, 6}, {13, 22}, {0, 0}, {0, 0}},
	{{4, 4}, {22, 4}, {4, 22}, {22, 22}, {0, 0}},
	{{2, 2}, {24, 2}, {13, 13}, {2, 24}, {24, 24}}
};

PinBox::PinBox(const int &pin_number, const QPoint &_position, QGraphicsItem *parent):
	Box(_position, parent)
{
	for(int i = 0; i < pin_number; ++i) {
		auto pin = new Pin(this);
        mPins.append(pin);
        pin->setPos(sPinPositions[pin_number-2][i][0], sPinPositions[pin_number-2][i][1]);
	}
	setState(Box::State::Future);
}

void PinBox::getValue(int &bl, int &wt) const
{
	bl = 0;
	wt = 0;
    foreach(Pin *pin, mPins) {
		switch (pin->getColor()) {
		case Pin::Color::White:
			++wt;
			break;
		case Pin::Color::Black:
			++bl;
			break;
		default:
			break;
		}
	}
}

void PinBox::setPins()
{
    for(int i = 0; i < Guess::instance()->blacks(); ++i)
        mPins.at(i)->setColor(Pin::Color::Black);

    for(int i = Guess::instance()->blacks(); i < Guess::instance()->blacks()+Guess::instance()->whites(); ++i)
        mPins.at(i)->setColor(Pin::Color::White);
}

void PinBox::setState(const Box::State &_state)
{
    mState = _state;
    bool pin_activity = (mState == Box::State::None);
    foreach(Pin *pin, mPins)
		pin->setActivity(pin_activity);
	update();
}

