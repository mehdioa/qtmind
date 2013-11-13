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

#ifndef EmptyBox_H
#define EmptyBox_H
enum BOX_STATE {
	BOX_PAST		= 0,
	BOX_CURRENT		= 1,
	BOX_FUTURE		= 2,
	BOX_NONE		= 3
};
#include <QGraphicsItem>

class EmptyBox : public QGraphicsItem
{
public:
	const static int colors_rgb[4][3];
	const static int alphas[4];
	EmptyBox(const QPoint& position = QPoint(0, 0), QGraphicsItem* parent = 0);
	virtual void setBoxState(const BOX_STATE& state = BOX_FUTURE);
	BOX_STATE getState() const {return mBoxState;}
	QRectF boundingRect() const;

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

	BOX_STATE mBoxState;
};

#endif // EmptyBox_H
