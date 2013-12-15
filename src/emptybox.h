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

#ifndef EmptyBox_H
#define EmptyBox_H

#include "constants.h"
#include <QGraphicsRectItem>

class EmptyBox : public QGraphicsRectItem
{
public:
	static const int BoxAlphas[4];

	explicit EmptyBox(const QPoint &position = QPoint(0, 0), QGraphicsItem *parent = 0);
	virtual void setBoxState(const BoxState &state = BoxState::Future);
	BoxState getBoxState() const {return mBoxState;}

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
	BoxState mBoxState;
};

#endif // EmptyBox_H