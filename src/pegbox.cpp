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

PegBox::PegBox(const QPoint &position, QGraphicsItem *parent):
	EmptyBox(position, parent),
	mPeg(0)
{

}
//-----------------------------------------------------------------------------

int PegBox::getPegColor() const
{
	if (mPeg)
		return mPeg->getColor();
	return -1;
}
//-----------------------------------------------------------------------------

bool PegBox::isPegVisible()
{
	return mPeg && mPeg->isVisible();
}

PegState PegBox::getPegState() const
{
	if (mPeg) return mPeg->getState();
	return PegState::Plain;
}
//-----------------------------------------------------------------------------

void PegBox::setBoxState(const BoxState &state)
{
	mBoxState = state;

	if (mPeg)
	{
		switch (mBoxState) {
		case BoxState::Past:
			mPeg->setState(PegState::Underneath);
			break;
		case BoxState::Current:
			mPeg->setState(PegState::Visible);
			break;
		case BoxState::Future:
			mPeg->setState(PegState::Invisible);
			break;
		default: // BoxState::None
			mPeg->setState(PegState::Invisible);
			break;
		}
	}
	update();
}
//-----------------------------------------------------------------------------

void PegBox::setPeg(Peg *peg)
{
	mPeg = peg;
	mPeg->setBox(this);
}
//-----------------------------------------------------------------------------

void PegBox::setPegColor(const int &color_number)
{
	if (mPeg)
		mPeg->setColor(color_number);
}
//-----------------------------------------------------------------------------

void PegBox::setPegState(const PegState &state)
{
	if (mPeg)
		mPeg->setState(state);
}
