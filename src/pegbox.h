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

#ifndef PEGBOX_H
#define PEGBOX_H

#include "box.h"
#include "peg.h"

//class Game;
class IPegConnector;

class PegBox : public Box
{
public:

    explicit PegBox(const QPoint& position, QGraphicsItem* parent = 0);

    /**
    * @brief hasPeg Does this box has a peg?
    * @return true if it has a peg, false otherwise
    */
    bool hasPeg() const {return mPeg != 0;}

    /**
    * @brief setPeg set the peg of this box
    * @param m_peg the peg of the box
    */
    void setPeg(Peg* peg) {mPeg = peg;}

    /**
    * @brief setPegState set the state of the peg of the box
    * @param peg_state the peg state
    */
    void setPegState(const Peg::State& peg_state);

    /**
    * @brief setPegColor set the peg color
    * @param _color the color number
    */
    void setPegColor(const int& color, IPegConnector* peg_connector);

    /**
    * @brief getPegColor get the color of the peg
    * @return the color of the peg of the box
    */
    int getPegColor() const;

    /**
    * @brief setState set the state of the box
    * @param _state the box state
    */
    void setState(const Box::State& state);

    /**
    * @brief isPegVisible is the peg visible?
    * @return true if the peg is visible, false otherwise
    */
    bool isPegVisible();

    /**
    * @brief getPegState get the state of the peg of the box
    * @return the state of the peg of the box
    */
    Peg::State getPegState() const;

    Peg* peg() const;

//    void createPeg(Game* game, const int& color);

//    void createPeg(const QPointF& position, const int& color);

//    void createPeg(const int& color);


private:
    Peg* mPeg; /**< TODO */
};

#endif // PEGBOX_H
