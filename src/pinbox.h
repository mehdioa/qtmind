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

#include "box.h"
#include "guess.h"
#include "rules.h"

class Pin;

/**
 * @brief The PinBox class represent a pin box on the board
 */
class PinBox : public Box
{
public:
	/**
	 * @brief PinBox
	 * @param pin_number
	 * @param m_position
	 * @param parent
	 */
	explicit PinBox(const int &pin_number, const QPoint &m_position, QGraphicsItem *parent = 0);

	/**
	 * @brief getValue get the value of the box
	 * @return the total value of the pinbox. Look the solver class for the explanation of the
	 * value.
	 */
	void getValue(int &bl, int &wt) const;

	/**
	 * @brief setPins set pins of the box
	 * @param codeA first code
	 * @param codeB second code
	 * @param color_n the number of colors
	 */
	void setPins(const Guess &guess, Rules *rules);

	/**
	 * @brief setState set the box state
	 * @param m_state the box state
	 */
	void setState(const Box::State &m_state = Box::State::Future);

private:
	static const int PinPositions[Rules::MAX_SLOT_NUMBER][Rules::MAX_SLOT_NUMBER][2]; /**< TODO */
	QVector<Pin *> pins; /**< TODO */
};

#endif // PINBOX_H
