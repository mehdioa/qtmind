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

#ifndef PEG_H
#define PEG_H

#include <QGraphicsEllipseItem>
#include "constants.h"

class QGraphicsDropShadowEffect;
class QGraphicsSimpleTextItem;
class PegBox;

class Peg : public QObject, public QGraphicsEllipseItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

public:
	explicit Peg(const QPoint &position, int color_number = 0,
	const IndicatorType &indicator_n = IndicatorType::None, QGraphicsItem *parent = 0);
	void setColor(const int &color_number);
	int getColor() const {return mColor;}
	void setMovable(bool );
	void setBox(PegBox *box) {mBox = box;}

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *);

signals:
	void mouseReleasedSignal(QPoint position, int color);

protected slots:
	void onChangeIndicators(const IndicatorType &indicator_n);

private:
	QPointF mPosition;// position of hole containing peg
	PegBox *mBox;
	IndicatorType mIndicatorType;

	/*	pressed shadow - the ellipse item takes ownership of
	 *	the effect, so no need to delete the pointer in the destructor*/
	QGraphicsDropShadowEffect *pressedEffect;
	QGraphicsSimpleTextItem *mIndicator;
	int mColor;
};

#endif // PEG_H
