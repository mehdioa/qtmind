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


Message::Message(const QString &color_name, const int &font_size):
mColor(QColor(color_name))
{
	mLayout.setFont(QFont("Linux Libertine", font_size, QFont::Bold, false));
	mLayout.setTextOption(QTextOption(Qt::AlignHCenter));
	mUpdateRect = QRectF(0, 0, 10, 10);

}

void Message::showMassage(const QString str)
{
	mUpdateRect = boundingRect();
	mLayout.setText(str);

	int leading = -3;
	qreal h = 0;
	qreal maxw = 0;
	qreal maxh = 0;
	mLayout.beginLayout();

	while (true)
	{
		QTextLine line = mLayout.createLine();
		if (!line.isValid())
		{
			break;
		}

		line.setLineWidth(280);
		h += leading;
		line.setPosition(QPointF(0, h));
		h += line.height();
		maxw = qMax(maxw, line.naturalTextWidth());
	}
	mLayout.endLayout();

	float ypos = 4 + (70 - mLayout.boundingRect().height()) / 2;

	maxw = qMax(mUpdateRect.width(), mLayout.boundingRect().width());
	maxh = qMax(mUpdateRect.height(), mLayout.boundingRect().height() + ypos);

	mUpdateRect = QRectF(0, 0, maxw, maxh + ypos);

	update(boundingRect());
}

QRectF Message::boundingRect() const
{
	return mUpdateRect;
}

void Message::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setRenderHint(QPainter::TextAntialiasing, true);
	painter->setPen(QPen(mColor));
	float ypos = 4 + (70 - mLayout.boundingRect().height()) / 2;
	mLayout.draw(painter, QPointF(0, ypos));
}
