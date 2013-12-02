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
	mPegState(PegState::Invisible),
	mPeg(0)
{
	QLinearGradient lgrad(2, 2, 35, 35);
	lgrad.setColorAt(0.0, QColor(0, 0, 0, 150));
	lgrad.setColorAt(1.0, QColor(255, 255, 255, 160));

	mCircle = new QGraphicsEllipseItem(2, 2, 35, 35, this);
	mCircle->setPen(QPen(QBrush(lgrad), 2));
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
//-----------------------------------------------------------------------------

void PegBox::setBoxState(const BoxState &state)
{
	mBoxState = state;

	switch (mBoxState) {
	case BoxState::Past:
		if(mPeg)
		{
			mPeg->setVisible(true);
			mPeg->setMovable(false);
		}
		mCircle->setVisible(true);
		break;
	case BoxState::Current:
		if (mPeg)
		{
			mPeg->setMovable(true);
		}
		break;
	default: // BoxState::None and BoxState::Future
		if (mPeg)
			mPeg->setVisible(false);
		break;
	}
	update();
}
//-----------------------------------------------------------------------------

void PegBox::setPeg(Peg *peg)
{
	mPeg = peg;
	mPeg->setBox(this);
	mCircle->setVisible(hasPeg());
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
	mPegState = state;
	switch (mPegState) {
	case PegState::Draged:
		mPeg->setVisible(true);
		mCircle->setVisible(false);
		break;
	case PegState::Invisible:
		if (mPeg)
			mPeg->setVisible(false);
		mCircle->setVisible(false);
		break;
	default:// PegState::Initial and PegState::Visible
		if (mPeg)
			mPeg->setVisible(true);
		mCircle->setVisible(true);
		break;
	}
}
