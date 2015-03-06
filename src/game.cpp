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

#include "game.h"
#include "rules.h"
#include "button.h"
#include "pegbox.h"
#include "pinbox.h"
#include "solver.h"
#include "message.h"
#include "tools.h"
#include "ctime"
#include <QSettings>

inline static void setStateOfList(QList<PegBox*>* boxlist, const Box::State& state_t)
{
    foreach (PegBox* box, *boxlist)
        box->setState(state_t);
}

Game::Game():
    QGraphicsView(),
    mState(State::None),
    mSolver(0)
{
    QSettings settings;
    Peg::setShowColors(settings.value("ShowColors", 1).toBool());
    Peg::setShowIndicators(settings.value("ShowIndicators", 0).toBool());
    Peg::setIndicator((Indicator) settings.value("Indicator", 65).toInt());

    auto scene = new QGraphicsScene(this);
    setScene(scene);
    scene->setSceneRect(0, 0, 320, 560);
    fitInView(sceneRect(), Qt::KeepAspectRatio);
    setCacheMode(QGraphicsView::CacheNone);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

Game::~Game()
{
    QSettings settings;
    settings.setValue("ShowColors", (int) Peg::getShowColors());
    settings.setValue("ShowIndicators", (int) Peg::getShowIndicators());
    settings.setValue("Indicator", (int) Peg::getIndicator());

    if (mSolver) {
        mSolver->interupt();
        mSolver->quit();
        mSolver->wait();
        mSolver->deleteLater();
    }

    scene()->clear();
}

void Game::codeRowFilled(const bool& filled)
{
    if(mRules->mode() == Mode::HVM) {
        mOkButton->setVisible(filled);
        mOkButton->setEnabled(filled);
        if (filled) {
            mState = State::WaittingPinboxPress;
            showMessage();

            if (mTools->mAutoCloseRows)
                onOkButtonPressed();
        } else {
            if (mMovesPlayed == 0)
                mState = State::WaittingFirstRowFill;
            else
                mState = State::WaittingCodeRowFill;

            showMessage();
        }
    } else {	//Rules::instance()->m_mode == Mode::MVH
        mDoneButton->setVisible(filled);

        if (filled)//the user is not done putting the master code
            mState = State::WaittingDoneButtonPress;
        else
            mState = State::WaittingHiddenCodeFill;

        showMessage();
    }
}

void Game::createBoxes()
{
    /*	This function creates boxes and put them in QLists. They are
         *	stored in QList from bottom to top and left to right.
         */
    mCodeBoxes.clear();
    mPinBoxes.clear();
    mPegBoxes.clear();
    mCurrentBoxes.clear();
    mMasterBoxes.clear();

    QPoint left_bottom_corner(4, 489);

    int pegs = mRules->pegs();
    int colors = mRules->colors();

    for (int i = 0; i < MAX_COLOR_NUMBER; ++i) {
        QPoint position = left_bottom_corner - QPoint(0, i*40);

        auto pinbox = new PinBox(pegs, position);
        scene()->addItem(pinbox);
        mPinBoxes.append(pinbox);

        position.setX(160-20*pegs);

        for (int j = 0; j < pegs; ++j) {
            mCodeBoxes.append(createPegBox(position+QPoint(j*40, 0)));
        }

        position.setX(277);	//go to right corner for the peg boxes

        PegBox* pegbox = createPegBox(position);
        mPegBoxes.append(pegbox);
        if (i < colors) {
            Peg* peg;
            peg = createPeg(pegbox, i);
            peg->setState(Peg::State::UNDERNEATH);
            connect(this, SIGNAL(resetIndicatorsSignal()), peg, SLOT(onResetIndicators()));
            createPegForBox(pegbox, i);
            pegbox->setPegState(Peg::State::INITIAL);
        } else {
            createPeg(pegbox, i)->setState(Peg::State::PLAIN);
        }
    }

    // the last code boxes are for the master code
    for (int i = 0; i < pegs; ++i) {
        mMasterBoxes.append(createPegBox(QPoint(160-20*pegs+i*40, 70)));
    }
}

void Game::createPegForBox(PegBox* box, int color)
{
    QPointF pos = box->sceneBoundingRect().center();
    if (box->hasPeg()) {
        box->setPegColor(color);
    } else {
        Peg* peg = createPeg(pos, color);
        box->setPeg(peg);
        connect(peg, SIGNAL(mouseReleaseSignal(Peg*)), this, SLOT(onPegMouseReleased(Peg*)));
        connect(peg, SIGNAL(mouseDoubleClickSignal(Peg*)), this, SLOT(onPegMouseDoubleClicked(Peg*)));
        connect(this, SIGNAL(resetIndicatorsSignal()), peg, SLOT(onResetIndicators()));
    }
}

PegBox* Game::createPegBox(const QPoint& position)
{
    auto pegbox = new PegBox(position);
    scene()->addItem(pegbox);
    return pegbox;
}

Peg* Game::createPeg(const QPointF& position, const int& color)
{
    auto peg = new Peg(position, color);
    scene()->addItem(peg);
    return peg;
}

Peg* Game::createPeg(PegBox* box, const int& color)
{
    return createPeg(box->sceneBoundingRect().center(), color);
}

void Game::freezeScene()
{
    setStateOfList(&mMasterBoxes, Box::State::PAST);
    setStateOfList(&mCurrentBoxes, Box::State::PAST);
    mPinBoxes.at(mMovesPlayed)->setState(Box::State::PAST);
    setStateOfList(&mPegBoxes, Box::State::FUTURE);
}

void Game::setNextRowInAction()
{
    int pegs = mRules->pegs();
    mCurrentBoxes.clear();
    for(int i = mMovesPlayed*pegs; i < (mMovesPlayed+1)*pegs; ++i)
        mCurrentBoxes.append(mCodeBoxes.at(i));
    setStateOfList(&mCurrentBoxes, Box::State::CURRENT);
    mState = State::WaittingCodeRowFill;
}

void Game::getNextGuess()
{
    mState = State::Thinking;
    mSolver->startGuessing(mRules->algorithm());
}

Game::Player Game::winner() const
{
    int blacks, whites;
    mPinBoxes.at(mMovesPlayed)->getValue(blacks, whites);
    if (blacks == mRules->pegs())
        return Player::CodeBreaker;
    else if (mMovesPlayed >= MAX_COLOR_NUMBER - 1)
        return Player::CodeMaker;
    else
        return Player::None;
}

void Game::initializeScene()
{
    mCodeBoxes.clear();
    mPinBoxes.clear();
    mPegBoxes.clear();
    mMasterBoxes.clear();
    mCurrentBoxes.clear();
    mMovesPlayed = 0;

    scene()->clear();
    setInteractive(true);

    mOkButton = new Button(mTools->mFontName, mTools->mFontSize, 36, tr("OK"));
    scene()->addItem(mOkButton);
    mOkButton->setZValue(2);
    connect(mOkButton, SIGNAL(buttonPressed()), this, SLOT(onOkButtonPressed()));
    mOkButton->setEnabled(false);
    mOkButton->setVisible(false);

    mDoneButton = new Button(mTools->mFontName, mTools->mFontSize, 158, tr("Done"));
    mDoneButton->setPos(79, 118);
    mDoneButton->setVisible(false);
    mDoneButton->setZValue(2);
    mDoneButton->setEnabled(false);
    connect(mDoneButton, SIGNAL(buttonPressed()), this, SLOT(onDoneButtonPressed()));
    scene()->addItem(mDoneButton);

    mMessage = new Message(mTools->mFontName, mTools->mFontSize, "#303030");
    scene()->addItem(mMessage);
    mMessage->setPos(20, 0);
    connect(this, SIGNAL(fontChangedSignal(QString,int)), mMessage, SLOT(onFontChanged(QString,int)));

    mInformation = new Message(mTools->mFontName, mTools->mFontSize, "#808080", 4);
    scene()->addItem(mInformation);
    mInformation->setPos(20, 506);
    connect(this, SIGNAL(fontChangedSignal(QString,int)), mInformation, SLOT(onFontChanged(QString,int)));
    showInformation();
    createBoxes();
    scene()->update();
}

void Game::onPegMouseReleased(Peg* peg)
{
    QPointF position = peg->sceneBoundingRect().center();
    int color = peg->getColor();
    //if same color is not allowed and there is already a color-peg visible, we just ignore drop
    if(!mRules->sameColors()) {
        foreach(PegBox* box, mCurrentBoxes) {
            if(!box->sceneBoundingRect().contains(position) &&
                    box->isPegVisible() && box->getPegColor() == color) {
                emit pegDropRefuseSignal();
                return;
            }
        }
    }

    static bool rowFillState = false;
    bool newRowFillState = true;

    // conversion from float to integer may cause double drop on middle. Flag to do it just once
    bool dropOnlyOnce = true;

    foreach(PegBox* box, mCurrentBoxes) {
        if (box->sceneBoundingRect().contains(position) && dropOnlyOnce) {
            dropOnlyOnce = false;
            createPegForBox(box, color);
            emit pegDropSignal();
            box->setPegState(Peg::State::VISIBLE);
        }//	end if

        if (box->getPegState() != Peg::State::VISIBLE)
            newRowFillState = false;
    } //	end foreach

    //	if (master) code row state changed, go to codeRowFilled
    if (newRowFillState != rowFillState) {
        rowFillState = newRowFillState;
        codeRowFilled(rowFillState);
    }
}

void Game::onPegMouseDoubleClicked(Peg* peg)
{
    foreach(PegBox* box, mCurrentBoxes) {
        if (!box->hasPeg() || box->getPegState() == Peg::State::INVISIBLE) {
            peg->setPos(box->sceneBoundingRect().topLeft());
            break;
        }
    }
}

void Game::onResetIndicators()
{
    emit resetIndicatorsSignal();
}

void Game::onFontChanged()
{
    emit fontChangedSignal(mTools->mFontName, mTools->mFontSize);
}

void Game::retranslateTexts()
{
    if(mOkButton) {
        mOkButton->setLabel(tr("OK"));
        mOkButton->update();
    }
    if (mDoneButton) {
        mDoneButton->setLabel(tr("Done"));
        mDoneButton->update();
    }
    showInformation();
    showMessage();
}

bool Game::isRunning()
{
    switch (mState) {
    case State::None:
        return false;
    case State::Win:
        return false;
    case State::Lose:
        return false;
    case State::Resign:
        return false;
    case State::WaittingFirstRowFill:
        return false;
    case State::WaittingHiddenCodeFill:
        return false;
    case State::WaittingDoneButtonPress:
        return false;
    default:
        return true;
    }
}

void Game::onRevealOnePeg()
{
    if (mRules->mode() == Mode::HVM && isRunning()) {
        foreach(PegBox* box, mMasterBoxes) {
            if(box->getState() != Box::State::PAST) {
                if (box == mMasterBoxes.last()) {
                    onResigned();
                } else {
                    box->setState(Box::State::PAST);
                    return;
                }
            }
        }
    }
}

void Game::onResigned()
{
    if (mRules->mode() == Mode::HVM && isRunning()) {
        mState = State::Resign;
        showMessage();
        freezeScene();
    }
}

void Game::onOkButtonPressed()
{
    if(mRules->mode() == Mode::MVH) {
        int blacks, whites;
        mPinBoxes.at(mMovesPlayed)->getValue(blacks, whites);
        if(!mSolver->setResponse(blacks, whites, mGuess.mGuess)) {
            mMessage->setText(tr("Not Possible, Try Again"));
            return;
        }
        emit buttonClickSignal();
        mOkButton->setVisible(false);

        mPinBoxes.at(mMovesPlayed)->setState(Box::State::PAST);

        switch (winner()) {
        case Player::CodeBreaker:
            mState = State::Win;
            freezeScene();
            break;
        case Player::CodeMaker:
            mState = State::Lose;
            freezeScene();
            break;
        default:
            ++mMovesPlayed;
            getNextGuess();
            break;
        }

        showMessage();
    } else {
        emit buttonClickSignal();
        mState = State::Running;

        unsigned char new_guess[MAX_SLOT_NUMBER];
        for(int i = 0; i < mRules->pegs(); ++i)
            new_guess[i] = mCurrentBoxes.at(i)->getPegColor();
        mGuess.setGuess(new_guess);

        mPinBoxes.at(mMovesPlayed)->setPins(mGuess.mBlacks, mGuess.mWhites);
        mPinBoxes.at(mMovesPlayed)->setState(Box::State::PAST);

        setStateOfList(&mCurrentBoxes, Box::State::PAST);
        mCurrentBoxes.clear();

        switch (winner()) {
        case Player::CodeBreaker:
            mState = State::Win;
            freezeScene();
            break;
        case Player::CodeMaker:
            mState = State::Lose;
            freezeScene();
            break;
        default:
            ++mMovesPlayed;
            setNextRowInAction();
            break;
        }

        showMessage();
        mOkButton->setVisible(false);
        mOkButton->setPos(mPinBoxes.at(mMovesPlayed)->pos() + QPoint(0, 1));
    }
}

void Game::onDoneButtonPressed()
{
    emit buttonClickSignal();
    mState = State::Running;

    mDoneButton->setVisible(false);
    mDoneButton->setEnabled(false);

    setStateOfList(&mCurrentBoxes, Box::State::PAST);
    setStateOfList(&mPegBoxes, Box::State::FUTURE);

    unsigned char new_guess[MAX_SLOT_NUMBER];
    for(int i = 0; i < mRules->pegs(); ++i)
        new_guess[i] = mCurrentBoxes.at(i)->getPegColor();
    mGuess.setCode(new_guess);

    mCurrentBoxes.clear();
    getNextGuess();
    showMessage();
}

void Game::onGuessReady()
{
    mState = State::Running;
    showInformation();

    int box_index = mMovesPlayed*mRules->pegs();
    for(int i = 0; i < mRules->pegs(); ++i) {
        createPegForBox(mCodeBoxes.at(box_index + i), mGuess.mGuess[i]);
        mCodeBoxes.at(box_index + i)->setState(Box::State::PAST);
    }
    mState = State::WaittingOkButtonPress;
    showMessage();
    mPinBoxes.at(mMovesPlayed)->setState(Box::State::NONE);
    mOkButton->setEnabled(true);
    mOkButton->setVisible(true);
    mOkButton->setPos(mPinBoxes.at(mMovesPlayed)->pos()-QPoint(0, 39));

    if (mTools->mAutoPutPins)
        mPinBoxes.at(mMovesPlayed)->setPins(mGuess.mBlacks, mGuess.mWhites);
}

void Game::play()
{
    stop();

    if(mRules->mode() == Mode::MVH)
        playMVH();
    else // (mRules->mode == Mode::HVM)
        playHVM();
}

void Game::stop()
{
    if (mSolver) {
        mSolver->interupt();
        mSolver->quit();
        mSolver->wait();
    }
    initializeScene();
    mState = State::None;
}

void Game::playMVH()
{
    mDoneButton->setZValue(2);
    mDoneButton->setVisible(false);
    mDoneButton->setEnabled(true);

    if (!mSolver) {
        mSolver = new Solver(&mGuess, this);
        connect(mSolver, SIGNAL(guessDoneSignal()), this, SLOT(onGuessReady()));

    }
    mSolver->interupt();
    mGuess.reset(mRules->colors(), mRules->pegs(), mRules->algorithm(), mSolver->reset(mRules->colors(), mRules->pegs(), mRules->sameColors()));

    mState = State::WaittingHiddenCodeFill;
    showMessage();
    showInformation();

    //initializing currentrow
    for(int i = 0; i < mRules->pegs(); ++i) {
        mCurrentBoxes.append(mMasterBoxes.at(i));
        mMasterBoxes.at(i)->setState(Box::State::CURRENT);
    }

    /*	Nothing happening from here till the user fill the master code
         *	row and press the done button. After the done button is pressed,
         *	the onm_doneButtonPressed is continuing the game
         */
}

void Game::playHVM()
{
    qsrand(time(NULL));
    int remaining_colors = mRules->colors();
    unsigned char hidden_code[MAX_SLOT_NUMBER];
    for(int i = 0; i < mRules->pegs(); i++) {
        hidden_code[i] = static_cast<unsigned char>(remaining_colors * (qrand()/(RAND_MAX + 1.0)));
        if (!mRules->sameColors()) {
            for(int j = 0; j < i; j++)
                if (hidden_code[j] <= hidden_code[i])
                    hidden_code[i]++;
            remaining_colors--;
        }
        createPegForBox(mMasterBoxes.at(i), hidden_code[i]);
        mMasterBoxes.at(i)->setState(Box::State::NONE);
    }

    mGuess.setCode(hidden_code);
    setNextRowInAction();
    showMessage();
    showInformation();
    mState = State::WaittingFirstRowFill;
    mOkButton->setPos(mPinBoxes.at(mMovesPlayed)->pos() + QPoint(0, 1));
    // from now on the onPinBoxPressed function continues the game, after the code row is filled
}

void Game::resizeEvent(QResizeEvent* event)
{
    fitInView(sceneRect(), Qt::KeepAspectRatio);
    QGraphicsView::resizeEvent(event);
}

void Game::showInformation()
{
    int& possibles = mGuess.mPossibles;
    qreal& weight = mGuess.mWeight;
    if (mRules->mode() == Mode::MVH) {
        if (possibles == 1)
        {
            mInformation->setText(tr("The Code Is Cracked!"));
        } else if (possibles > 10000) {
            mInformation->setText(QString("%1    %2: %3").arg(tr("Random Guess")).
                                  arg(tr("Remaining")).arg(mTools->mLocale.toString(possibles)));
        } else {
            switch (mGuess.mAlgorithm) {
            case Algorithm::MOST_PARTS:
                mInformation->setText(QString("%1: %2    %3: %4").arg(tr("Most Parts")).
                                      arg(mTools->mLocale.toString(weight)).arg(tr("Remaining")).
                                      arg(mTools->mLocale.toString(possibles)));
                break;
            case Algorithm::WORST_CASE:
                mInformation->setText(QString("%1: %2    %3: %4").arg(tr("Worst Case")).
                                      arg(mTools->mLocale.toString(weight)).arg(tr("Remaining")).
                                      arg(mTools->mLocale.toString(possibles)));
                break;
            default:
                mInformation->setText(QString("%1: %2    %3: %4").arg(tr("Expected Size")).
                                      arg(mTools->mLocale.toString(weight)).arg(tr("Remaining")).
                                      arg(mTools->mLocale.toString(possibles)));
                break;
            }

        }
    } else {
        mInformation->setText(QString("%1: %2   %3: %4   %5: %6").arg(tr("Slots", "", mRules->pegs())).
                              arg(mTools->mLocale.toString(mRules->pegs())).arg(tr("Colors", "", mRules->colors())).
                              arg(mTools->mLocale.toString(mRules->colors())).arg(tr("Same Colors")).
                              arg(mRules->sameColors() ? tr("Yes"): tr("No")));
    }
}

void Game::setRules(Rules* rules)
{
    mRules = rules;
}

void Game::setTools(Tools* tools)
{
    mTools = tools;
}

void Game::showMessage()
{
    bool is_MVH = (mRules->mode() == Mode::MVH);
    switch (mState) {
    case State::Win:
        if (is_MVH)
            mMessage->setText(tr("Success! I Win"));
        else
            mMessage->setText(tr("Success! You Win"));
        break;
    case State::Lose:
        if (is_MVH)
            mMessage->setText(tr("Game Over! I Failed"));
        else
            mMessage->setText(tr("Game Over! You Failed"));
        break;
    case State::Resign:
        mMessage->setText(tr("You Resign"));
        break;
    case State::Thinking:
        mMessage->setText(tr("Let Me Think"));
        break;
    case State::WaittingPinboxPress:
        mMessage->setText(tr("Press OK"));
        break;
    case State::WaittingOkButtonPress:
        mMessage->setText(tr("Please Put Your Pins And Press OK"));
        break;
    case State::WaittingDoneButtonPress:
        mMessage->setText(tr("Press Done"));
        break;
    default:
        mMessage->setText(tr("Place Your Pegs"));
        break;
    }
}
void Game::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, QColor(200, 200, 200));// set scene background color
    painter->setPen(Qt::NoPen);

    QRectF cr(3, 3, 314, 554);
    QPainterPath cpath;
    cpath.addRoundedRect(cr, 10.1, 10.1);
    painter->setClipPath(cpath);
    painter->setClipping(true);

    QLinearGradient top_grad(0, 16, 0, 129);
    top_grad.setColorAt(0.0, QColor(248, 248, 248));
    top_grad.setColorAt(0.6, QColor(184, 184, 184));
    top_grad.setColorAt(1, QColor(212, 212, 212));
    painter->setBrush(QBrush(top_grad));
    painter->drawRect(QRect(4, 4, 312, 125));
    painter->setBrush(QBrush(QColor(112, 112, 112)));
    painter->drawRect(QRect(4, 128, 318, 1));

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor(150, 150, 150)));
    painter->drawRect(QRectF(4, 129, 318, 400));

    QLinearGradient bot_grad(0, 530, 0, 557);
    bot_grad.setColorAt(0.0, QColor(204, 204, 204));
    bot_grad.setColorAt(0.3, QColor(206, 206, 206));
    bot_grad.setColorAt(1.0, QColor(180, 180, 180));
    painter->setBrush(QBrush(bot_grad));
    painter->drawRect(QRect(1, 529, 318, 28));
    painter->setBrush(QBrush(QColor(239, 239, 239)));
    painter->setClipping(false);

    QLinearGradient frame_grad(0, 190, 320, 370);
    frame_grad.setColorAt(0.0, QColor(240, 240, 240));
    frame_grad.setColorAt(0.49, QColor(240, 240, 240));
    frame_grad.setColorAt(0.50, QColor(80, 80, 80));
    frame_grad.setColorAt(1.0, QColor(80, 80, 80));
    QPen frame_pen = QPen(QBrush(frame_grad), 1.5);
    QRectF right_shadow(3.5, 3.5, 313, 553);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(frame_pen);
    painter->drawRoundedRect(right_shadow, 9.8, 9.8);

    QLinearGradient sol_frame_grad = QLinearGradient(50, 70, 50, 110);
    sol_frame_grad.setColorAt(0.0, QColor(80, 80, 80));
    sol_frame_grad.setColorAt(1.0, QColor(255, 255, 255));
    QPen sol_frame_pen(QBrush(sol_frame_grad), 1);

    painter->setPen(sol_frame_pen);

    QRectF sol_container(41, 68, 235, 42);
    QRectF sol_frame(42, 69, 235, 41.5);

    painter->drawRoundedRect(sol_container, 21,21);
    painter->setBrush(QColor(150, 150, 150));
    painter->drawRoundedRect(sol_frame, 20, 20);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
}
