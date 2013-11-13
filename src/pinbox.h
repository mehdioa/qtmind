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

#ifndef PINBOX_H
#define PINBOX_H

#include "emptybox.h"
class Pin;

class PinBox : public QObject, public EmptyBox
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

public:
	PinBox(const int& pin_number, const QPoint& position, QGraphicsItem* parent = 0);
	static const int pin_positions[5][5][2];
	int getValue(int& blacks, int& whites) const;
	int getValue() const;
	void setPins(const int& b, const int& w);
	void setPins(const QString& codeA, const QString& codeB, const int& peg, const int& color);
	void setState(const BOX_STATE& state = BOX_FUTURE);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *);

signals:
	void pinBoxPushed();

private:
	QVector<Pin*> pins;
};

#endif // PINBOX_H
