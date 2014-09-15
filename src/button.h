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

#include <QGraphicsObject>
#include <QFont>

class QGraphicsDropShadowEffect;

/**
 * @brief A class to represent ok and done buttons on the board
 *
 */
class Button : public QGraphicsObject
{
	Q_OBJECT

public:

	/**
	 * @brief Button creates a button
	 * @param board_font font of the button
	 * @param buttonWidth width of the button
	 * @param str label of the button
	 */
	explicit Button(const int &buttonWidth = 152, const QString &str = "", QGraphicsItem *parent = 0);

	/**
	 * @brief setLabel set the label of the button
	 * @param label_s the label
	 */
	void setLabel(QString _label){m_label = _label;}

protected:
	/**
	 * @brief mousePressEvent
	 */
	void mousePressEvent(QGraphicsSceneMouseEvent *);
	/**
	 * @brief mouseReleaseEvent
	 * @param event
	 */
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	/**
	 * @brief paint
	 * @param painter
	 */
	void paint(QPainter *painter, const QStyleOptionGraphicsItem*,
			   QWidget*);
	/**
	 * @brief boundingRect
	 * @return
	 */
	QRectF boundingRect() const;

signals:
	/**
	 * @brief buttonPressed
	 */
	void buttonPressed();

private:
	QString m_label; /**< TODO */
	QFont m_font; /**< TODO */
	int m_width; /**< TODO */
	QGraphicsDropShadowEffect *m_pressedEffect; /**< TODO */
};

#endif // BUTTON_H
