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
#include <QTextLayout>
#include <QPainter>
#include <QFont>

Message::Message(const QString &color_name, const QString &font_name, const int &font_size, QGraphicsItem *parent):
	QGraphicsSimpleTextItem(parent),
	mColor(QColor(color_name))
{
	mLayout.setFont(QFont(font_name, font_size, QFont::Bold, false));
	mLayout.setTextOption(QTextOption(Qt::AlignHCenter));
	mUpdateRect = QRectF(0, 0, 10, 10);
}
//-----------------------------------------------------------------------------

void Message::showMessage(const QString str)
{
	mUpdateRect = boundingRect();
	mLayout.setText(str);

	int leading = -3;
	qreal height = 0;
	qreal max_width = 0;
	qreal max_height = 0;
	mLayout.beginLayout();

	QTextLine line = mLayout.createLine();
	while (line.isValid())
	{
		line.setLineWidth(280);
		height += leading;
		line.setPosition(QPointF(0, height));
		height += line.height();
		max_width = qMax(max_width, line.naturalTextWidth());
		line = mLayout.createLine();
	}
	mLayout.endLayout();

	float ypos = 4 + (70 - mLayout.boundingRect().height()) / 2;

	max_width = qMax(mUpdateRect.width(), mLayout.boundingRect().width());
	max_height = qMax(mUpdateRect.height(), mLayout.boundingRect().height() + ypos);

	mUpdateRect = QRectF(0, 0, max_width, max_height + ypos);

	update();
}
//-----------------------------------------------------------------------------

QRectF Message::boundingRect() const
{
	return mUpdateRect;
}
//-----------------------------------------------------------------------------

void Message::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setRenderHint(QPainter::TextAntialiasing, true);
	painter->setPen(QPen(mColor));
	float ypos = 4 + (70 - mLayout.boundingRect().height()) / 2;
	mLayout.draw(painter, QPointF(0, ypos));
}
