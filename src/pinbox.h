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

#ifndef PINBOX_H
#define PINBOX_H

#include "emptybox.h"
class Pin;

class PinBox : public QObject, public EmptyBox
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

public:
	explicit PinBox(const int &pin_number, const QPoint &position, QGraphicsItem *parent = 0);
	int getValue(int &blacks, int &whites) const;
	int getValue() const;
	void setPins(const int &b, const int &w);
	void setPins(const QString &codeA, const QString &codeB, const int &peg, const int &color);
	void setBoxState(const BoxState &state = BoxState::Future);
	void fakePress() {emit pinBoxPressed();}

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *);

signals:
	void pinBoxPressed();

private:
	QVector<Pin*> pins;
	bool onDemand;
};

#endif // PINBOX_H
