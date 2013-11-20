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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QGraphicsTextItem>
#include <QTextLayout>

class Message : public QGraphicsTextItem
{
public:
	Message(QString color_name = "#303133");
	void showMassage(const QString str);
	QRectF boundingRect() const;

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

private:
	QTextLayout mLayout;
	QRectF mUpdateRect;
	QColor mColor;
};

#endif // MESSAGE_H
