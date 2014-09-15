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

#ifndef BOX_H
#define BOX_H

#include <QGraphicsRectItem>

/**
 * @brief A class to represent an empty box, used in pinbox and pegbox
 *
 */
class Box : public QGraphicsRectItem
{
public:

	/**
	 * @brief The State enum represents different states of	a box
	 */
	enum class State {
		Past,
		Current,
		Future,
		None
	};

	/**
	 * @brief creates a box
	 * @param position the position of the box
	 * @param parent the parrent of the object
	 */
	explicit Box(const QPoint &position = QPoint(0, 0), QGraphicsItem *parent = 0);

	/**
	 * @brief sets the state of the box
	 * @param _state the state
	 */
	virtual void setState(const Box::State &_state = Box::State::Future);

	/**
	 * @brief gets the state of the box
	 * @return Box::State
	 */
	Box::State getState() const {return m_state;}

protected:
	/**
	 * @brief paint
	 * @param painter
	 */
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
	Box::State m_state; /**< The state of the box */

private:
	/**
	 * @brief BoxAlphas different alphas of the box
	 */
	static const int s_boxAlphas[4];
};

#endif // Box_H
