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

#ifndef BUTTON_H
#define BUTTON_H

#include <QGraphicsItem>
#include <QFont>
#include <QBrush>
class Board;

class Button : public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

public:
	Button(int buttonWidth = 152, QString str = "");
	void setPushable(bool);
	void fakeButtonPress(){emit buttonPressed();}

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent*);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

	void paint(QPainter* painter, const QStyleOptionGraphicsItem*,
			   QWidget*);
	QRectF boundingRect() const;

signals:
	void buttonPressed();

private:
	QString mLabel;
	QFont mFont;
	int mWidth;
	int mYOffs;

	QBrush mFillOutBrush;
	QBrush mFillOverBrush;
	QBrush* mFillBrush;

	QBrush mFrameOutBrush;
	QBrush mFrameOverBrush;
	QBrush* mFrameBrush;

	QRectF mRect;
	QRectF mRectFill;

};

#endif // BUTTON_H
