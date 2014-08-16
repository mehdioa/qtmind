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

class Board;

/**
 * @brief The Message class The class represent a message on the board.
 */
class Message : public QGraphicsSimpleTextItem
{
public:

	/**
	 * @brief Message creates a message
	 * @param board_font the font
	 * @param color_name the color
	 * @param smaller if a smaller font size should be used
	 * @param parent parent of the object
	 */
	explicit Message(Board *board, const QString &color_name = "#303133",
					 const int &smaller = 0, QGraphicsItem *parent = 0);

	/**
	 * @brief setText set the text of the message
	 * @param m_text the message text
	 */
	void setText(const QString m_text);

protected:
	/**
	 * @brief paint
	 * @param painter
	 */
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

	/**
	 * @brief boundingRect
	 * @return
	 */
	QRectF boundingRect() const;

private:
	QTextLayout textLayout; /**< TODO */
	QRectF updateRect; /**< TODO */
	QColor color; /**< TODO */
	QString text; /**< TODO */
};

#endif // MESSAGE_H
