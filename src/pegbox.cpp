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

#include "pegbox.h"
#include "ipegconnector.h"
#include "QGraphicsEllipseItem"
#include <QPen>
#include <QLinearGradient>

PegBox::PegBox(const QPoint& position, QGraphicsItem* parent):
    Box(position, parent),
    mPeg(0)
{

}

int PegBox::getPegColor() const
{
    if (mPeg)
        return mPeg->getColor();

    qDebug("getPegColor called with no peg setted.");
    return -1;
}

bool PegBox::isPegVisible()
{
    return mPeg && mPeg->isVisible();
}

Peg::State PegBox::getPegState() const
{
    if (mPeg) return mPeg->getState();
    return Peg::State::PLAIN;
}

Peg *PegBox::peg() const
{
    return mPeg;
}

//void PegBox::createPeg(Game *game, const int &color)
//{
//    if (mPeg) {
//        box->setPegColor(color);
//    } else {
//        Peg* peg = createPeg(sceneBoundingRect().center(), color);
//        box->setPeg(peg);
//        connect(peg, SIGNAL(mouseReleaseSignal(Peg*)), this, SLOT(onPegMouseReleased(Peg*)));
//        connect(peg, SIGNAL(mouseDoubleClickSignal(Peg*)), this, SLOT(onPegMouseDoubleClicked(Peg*)));
//        connect(this, SIGNAL(resetIndicatorsSignal()), peg, SLOT(onResetIndicators()));
//    }
//}

//void PegBox::createPeg(const QPointF &position, const int &color)
//{
//    auto peg = new Peg(position, color);
//    scene()->addItem(peg);
//    return peg;
//}

//void PegBox::createPeg(const int &color)
//{
//    createPeg(sceneBoundingRect().center(), color);

//}

void PegBox::setState(const Box::State& state)
{
    mState = state;

    if (mPeg) {
        switch (mState) {
        case Box::State::PAST:
            mPeg->setState(Peg::State::UNDERNEATH);
            break;
        case Box::State::CURRENT:
            mPeg->setState(Peg::State::VISIBLE);
            break;
        default: // Box::State::None, Box::State::Future
            mPeg->setState(Peg::State::INVISIBLE);
            break;
        }
    }
    update();
}

void PegBox::setPegColor(const int& color, IPegConnector *peg_connector)
{
    if (mPeg)
        mPeg->setColor(color);
    else {
        mPeg = new Peg(sceneBoundingRect().center(), color);
        peg_connector->connectPegToGame(mPeg);
    }
}

void PegBox::setPegState(const Peg::State& peg_state)
{
    if (mPeg)
        mPeg->setState(peg_state);
    else
        qDebug("setState called with no peg setted.");

}
