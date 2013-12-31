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

#ifndef BUTTON_H
#define BUTTON_H

#include <QGraphicsItem>
#include <QFont>
#include <QBrush>

class Board;
class BoardFont;
class QGraphicsDropShadowEffect;

class Button : public QObject, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

public:
	explicit Button(const BoardFont &board_font, const int &buttonWidth = 152, const QString &str = "");
	void fakeButtonPress(){emit buttonPressed();}
	void setLabel(QString label_s){label = label_s;}

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void paint(QPainter *painter, const QStyleOptionGraphicsItem*,
			   QWidget*);
	QRectF boundingRect() const;

signals:
	void buttonPressed();

private:
	QString label;
	QFont font;
	int width;
	QGraphicsDropShadowEffect *pressedEffect;
};

#endif // BUTTON_H
