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

#include <QGraphicsSimpleTextItem>
#include <QTextLayout>

class BoardFont;

class Message : public QGraphicsSimpleTextItem
{
public:
	explicit Message(const BoardFont &board_font, const QString &color_name = "#303133",
					 const int &smaller = 0, QGraphicsItem *parent = 0);
	void setText(const QString m_text);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
	QRectF boundingRect() const;

private:
	QTextLayout textLayout;
	QRectF updateRect;
	QColor color;
	QString text;
};

#endif // MESSAGE_H
