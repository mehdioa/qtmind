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

#ifndef PEGBOX_H
#define PEGBOX_H

#include "emptybox.h"
#include <QGraphicsItem>

class Peg;
class QGraphicsEllipseItem;
class Board;

class PegBox : public EmptyBox
{
public:
	explicit PegBox(const QPoint& position, QGraphicsItem* parent = 0);
	bool hasPeg() const {return mPeg != 0;}
	void setPeg(Peg* peg);
	void setPegState(const PegState& state = PegState::Empty);
	void setPegColor(int color_number);
	int getPegColor();
	void setBoxState(const BoxState& state = BoxState::Future);
	bool isPegVisible();

	PegState getPegState() const {return mPegState;}

private:
	PegState mPegState;
	QGraphicsEllipseItem* mCircle;
	Peg* mPeg;
};

#endif // PEGBOX_H
