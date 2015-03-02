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
	mColor(QColor(color_name)),
	mText("")
{
    mTextLayout.setFont(QFont(Board::instance().getFontName(), Board::instance().getFontSize()
                               - smaller, QFont::Bold, false));
	mTextLayout.setTextOption(QTextOption(Qt::AlignHCenter));
	mUpdateRect = QRectF(0, 0, 10, 10);
}

void Message::setText(const QString _text)
{
	mText = _text;
	mUpdateRect = boundingRect();
	mTextLayout.setText(mText);

	int leading = -3;
	qreal height = 0;
	qreal max_width = 0;
	qreal max_height = 0;
	mTextLayout.beginLayout();

	QTextLine line = mTextLayout.createLine();
	while (line.isValid()) {
		line.setLineWidth(280);
		height += leading;
		line.setPosition(QPointF(0, height));
		height += line.height();
		max_width = qMax(max_width, line.naturalTextWidth());
		line = mTextLayout.createLine();
	}
	mTextLayout.endLayout();

	float ypos = 4 + (70 - mTextLayout.boundingRect().height()) / 2;

	max_width = qMax(mUpdateRect.width(), mTextLayout.boundingRect().width());
	max_height = qMax(mUpdateRect.height(), mTextLayout.boundingRect().height() + ypos);

	mUpdateRect = QRectF(0, 0, max_width, max_height + ypos);

	update();
}

QRectF Message::boundingRect() const
{
	return mUpdateRect;
}

void Message::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setRenderHint(QPainter::TextAntialiasing, true);
	painter->setPen(QPen(mColor));
	float ypos = 4 + (70 - mTextLayout.boundingRect().height()) / 2;
	mTextLayout.draw(painter, QPointF(0, ypos));
}
