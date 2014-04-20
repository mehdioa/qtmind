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

#include "message.h"
#include "boardfont.h"
#include <QTextLayout>
#include <QPainter>
#include <QFont>

Message::Message(const BoardFont &board_font, const QString &color_name,
				 const int &smaller, QGraphicsItem *parent):
	QGraphicsSimpleTextItem(parent),
	color(QColor(color_name)),
	text("")
{
	textLayout.setFont(QFont(board_font.fontName, board_font.fontSize - smaller, QFont::Bold, false));
	textLayout.setTextOption(QTextOption(Qt::AlignHCenter));
	updateRect = QRectF(0, 0, 10, 10);
}
//-----------------------------------------------------------------------------

void Message::setText(const QString m_text)
{
	text = m_text;
	updateRect = boundingRect();
	textLayout.setText(text);

	int leading = -3;
	qreal height = 0;
	qreal max_width = 0;
	qreal max_height = 0;
	textLayout.beginLayout();

	QTextLine line = textLayout.createLine();
	while (line.isValid()) {
		line.setLineWidth(280);
		height += leading;
		line.setPosition(QPointF(0, height));
		height += line.height();
		max_width = qMax(max_width, line.naturalTextWidth());
		line = textLayout.createLine();
	}
	textLayout.endLayout();

	float ypos = 4 + (70 - textLayout.boundingRect().height()) / 2;

	max_width = qMax(updateRect.width(), textLayout.boundingRect().width());
	max_height = qMax(updateRect.height(), textLayout.boundingRect().height() + ypos);

	updateRect = QRectF(0, 0, max_width, max_height + ypos);

	update();
}
//-----------------------------------------------------------------------------

QRectF Message::boundingRect() const
{
	return updateRect;
}
//-----------------------------------------------------------------------------

void Message::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setRenderHint(QPainter::TextAntialiasing, true);
	painter->setPen(QPen(color));
	float ypos = 4 + (70 - textLayout.boundingRect().height()) / 2;
	textLayout.draw(painter, QPointF(0, ypos));
}
