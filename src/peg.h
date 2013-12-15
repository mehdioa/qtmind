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

enum class PegState
{
	Initial,
	Visible,
	Invisible,
	Underneath,
	Plain
};

class QGraphicsDropShadowEffect;
class QGraphicsSimpleTextItem;

class Peg : public QObject, public QGraphicsEllipseItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)

public:
	static const QColor PegColors[MAX_COLOR_NUMBER][2];
	static const QString OrderedChars[3];

	explicit Peg(const QPointF &position, int color_number = 0,
	const IndicatorType &indicator_n = IndicatorType::Color, QGraphicsItem *parent = 0);
	void setColor(const int &color_number);
	int getColor() const {return mColor;}
	void setMovable(bool );
	bool isMovable() const {return isActive;}
	void setState(const PegState &state);
	PegState getState() const {return mState;}

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);

signals:
	void mouseReleaseSignal(Peg *);
	void mouseDoubleClickSignal(Peg *);

protected slots:
	void onIndicatorChanged(const IndicatorType &indicator_n);

private:
//	PegBox *mBox;
	QPointF mPosition;
	IndicatorType mIndicatorType;

	/*	pressed shadow - the ellipse item takes ownership of
	 *	the effect, so no need to delete the pointer in the destructor*/
	QGraphicsDropShadowEffect *pressedEffect;
	QGraphicsSimpleTextItem *mIndicator;
	QGraphicsEllipseItem *mGloss;
	QGraphicsEllipseItem *mCircle;

	PegState mState;
	int mColor;
	bool isActive;
};

#endif // PEG_H
