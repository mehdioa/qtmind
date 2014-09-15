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
#include "board.h"
#include <QTextLayout>
#include <QPainter>
#include <QFont>

Message::Message(const QString &color_name,
				 const int &smaller, QGraphicsItem *parent):
	QGraphicsSimpleTextItem(parent),
	m_color(QColor(color_name)),
	m_text("")
{
	m_textLayout.setFont(QFont(Board::instance()->m_fontName, Board::instance()->m_fontSize - smaller, QFont::Bold, false));
	m_textLayout.setTextOption(QTextOption(Qt::AlignHCenter));
	m_updateRect = QRectF(0, 0, 10, 10);
}

void Message::setText(const QString _text)
{
	m_text = _text;
	m_updateRect = boundingRect();
	m_textLayout.setText(m_text);

	int leading = -3;
	qreal height = 0;
	qreal max_width = 0;
	qreal max_height = 0;
	m_textLayout.beginLayout();

	QTextLine line = m_textLayout.createLine();
	while (line.isValid()) {
		line.setLineWidth(280);
		height += leading;
		line.setPosition(QPointF(0, height));
		height += line.height();
		max_width = qMax(max_width, line.naturalTextWidth());
		line = m_textLayout.createLine();
	}
	m_textLayout.endLayout();

	float ypos = 4 + (70 - m_textLayout.boundingRect().height()) / 2;

	max_width = qMax(m_updateRect.width(), m_textLayout.boundingRect().width());
	max_height = qMax(m_updateRect.height(), m_textLayout.boundingRect().height() + ypos);

	m_updateRect = QRectF(0, 0, max_width, max_height + ypos);

	update();
}

QRectF Message::boundingRect() const
{
	return m_updateRect;
}

void Message::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setRenderHint(QPainter::TextAntialiasing, true);
	painter->setPen(QPen(m_color));
	float ypos = 4 + (70 - m_textLayout.boundingRect().height()) / 2;
	m_textLayout.draw(painter, QPointF(0, ypos));
}
