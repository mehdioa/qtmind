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

#ifndef PIN_H
#define PIN_H

#include <QGraphicsEllipseItem>

/**
 * @brief The Pin class represents a pin on the board
 */
class Pin : public QGraphicsEllipseItem
{
public:
	/**
	 * @brief The Color enum represent the color of the pin
	 */
	enum class Color {
		White,
		Black,
		None,
	};

	explicit Pin(QGraphicsItem *parent = 0);

	/**
	 * @brief getColor get the color of the pin
	 * @return the color of the pin
	 */
	Pin::Color getColor() const {return color;}

	/**
	 * @brief setColor set the color of the pin
	 * @param m_color the color of the pin
	 */
	void setColor(const Pin::Color &m_color);

	/**
	 * @brief setActivity set activity of the pin
	 * @param b the activity
	 */
	void setActivity(const bool &b);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);

private:
	Pin::Color nextPinColor() const;

private:
	Pin::Color color; /**< TODO */
	bool activity; /**< TODO */
};

#endif // PIN_H
