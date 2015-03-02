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
#include "board.h"
#include "button.h"
#include "pegbox.h"
#include "pinbox.h"
#include "solver.h"
#include "message.h"
#include "ctime"

inline static void setStateOfList(QList<PegBox *> *boxlist, const Box::State &state_t)
{
	foreach (PegBox *box, *boxlist)
		box->setState(state_t);
}

Game::Game():
	QGraphicsView(),
    mState(State::None),
    mSolver(0)
{
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
    if (mSolver) {
        mSolver->interupt();
        mSolver->quit();
        mSolver->wait();
        mSolver->deleteLater();
	}

	scene()->clear();
}

void Game::codeRowFilled(const bool &_filled)
{
    if(Rules::instance()->mode() == Mode::HVM) {
        mOkButton->setVisible(_filled);
        mOkButton->setEnabled(_filled);
		if (_filled) {
            mState = State::WaittingPinboxPress;
			showMessage();

            if (Board::instance()->isAutoCloseRows())
				onOkButtonPressed();
		} else {
            if (mMovesPlayed == 0)
                mState = State::WaittingFirstRowFill;
			else
                mState = State::WaittingCodeRowFill;

			showMessage();
		}
    } else {	//Rules::instance()->m_mode == Mode::MVH
        mDoneButton->setVisible(_filled);

		if (_filled)//the user is not done putting the master code
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

    int pegs = Rules::instance()->pegs();
    int colors = Rules::instance()->colors();

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

		PegBox *pegbox = createPegBox(position);
        mPegBoxes.append(pegbox);
        if (i < colors) {
			Peg *peg;
			peg = createPeg(pegbox, i);
			peg->setState(Peg::State::Underneath);
			connect(this, SIGNAL(showIndicatorsSignal()), peg, SLOT(onShowIndicators()));
			createPegForBox(pegbox, i);
			pegbox->setPegState(Peg::State::Initial);
		} else {
			createPeg(pegbox, i)->setState(Peg::State::Plain);
		}
	}

	// the last code boxes are for the master code
    for (int i = 0; i < pegs; ++i) {
        mMasterBoxes.append(createPegBox(QPoint(160-20*pegs+i*40, 70)));
	}
}

void Game::createPegForBox(PegBox *_box, int _color)
{
	QPointF pos = _box->sceneBoundingRect().center();
	if (_box->hasPeg()) {
		_box->setPegColor(_color);
	} else {
		Peg *peg = createPeg(pos, _color);
		_box->setPeg(peg);
		connect(peg, SIGNAL(mouseReleaseSignal(Peg *)), this, SLOT(onPegMouseReleased(Peg *)));
		connect(peg, SIGNAL(mouseDoubleClickSignal(Peg*)), this, SLOT(onPegMouseDoubleClicked(Peg*)));
		connect(this, SIGNAL(showIndicatorsSignal()), peg, SLOT(onShowIndicators()));
	}
}

PegBox *Game::createPegBox(const QPoint &_position)
{
	auto pegbox = new PegBox(_position);
	scene()->addItem(pegbox);
	return pegbox;
}

Peg *Game::createPeg(const QPointF &_position, const int &_color)
{
	auto peg = new Peg(_position, _color);
	scene()->addItem(peg);
	return peg;
}

Peg *Game::createPeg(PegBox *_box, const int &_color)
{
	return createPeg(_box->sceneBoundingRect().center(), _color);
}

void Game::freezeScene()
{
    setStateOfList(&mMasterBoxes, Box::State::Past);
    setStateOfList(&mCurrentBoxes, Box::State::Past);
    mPinBoxes.at(mMovesPlayed)->setState(Box::State::Past);
    setStateOfList(&mPegBoxes, Box::State::Future);
}

void Game::setNextRowInAction()
{
    int pegs = Rules::instance()->pegs();
    mCurrentBoxes.clear();
    for(int i = mMovesPlayed*pegs; i < (mMovesPlayed+1)*pegs; ++i)
        mCurrentBoxes.append(mCodeBoxes.at(i));
    setStateOfList(&mCurrentBoxes, Box::State::Current);
    mState = State::WaittingCodeRowFill;
}

void Game::getNextGuess()
{
    mState = State::Thinking;
    mSolver->startGuessing();
}

Game::Player Game::winner() const
{
	int blacks, whites;
    mPinBoxes.at(mMovesPlayed)->getValue(blacks, whites);
    if (blacks == Rules::instance()->pegs())
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

    mOkButton = new Button(36, tr("OK"));
    scene()->addItem(mOkButton);
    mOkButton->setZValue(2);
    connect(mOkButton, SIGNAL(buttonPressed()), this, SLOT(onOkButtonPressed()));
    mOkButton->setEnabled(false);
    mOkButton->setVisible(false);

    mDoneButton = new Button(158, tr("Done"));
    mDoneButton->setPos(79, 118);
    mDoneButton->setVisible(false);
    mDoneButton->setZValue(2);
    mDoneButton->setEnabled(false);
    connect(mDoneButton, SIGNAL(buttonPressed()), this, SLOT(onDoneButtonPressed()));
    scene()->addItem(mDoneButton);

    mMessage = new Message("#303030");
    scene()->addItem(mMessage);
    mMessage->setPos(20, 0);

    mInformation = new Message("#808080", 4);
    scene()->addItem(mInformation);
    mInformation->setPos(20, 506);
	showInformation();
	createBoxes();
	scene()->update();
}

void Game::onPegMouseReleased(Peg *peg)
{
	QPointF position = peg->sceneBoundingRect().center();
	int color = peg->getColor();
	//if same color is not allowed and there is already a color-peg visible, we just ignore drop
    if(!Rules::instance()->sameColors()) {
        foreach(PegBox *box, mCurrentBoxes) {
			if(!box->sceneBoundingRect().contains(position) &&
					box->isPegVisible() && box->getPegColor() == color) {
                Board::instance()->play(Sound::PegDropRefuse);
				return;
			}
		}
	}

	static bool rowFillState = false;
	bool newRowFillState = true;

	// conversion from float to integer may cause double drop on middle. Flag to do it just once
	bool dropOnlyOnce = true;

    foreach(PegBox *box, mCurrentBoxes) {
		if (box->sceneBoundingRect().contains(position) && dropOnlyOnce) {
			dropOnlyOnce = false;
			createPegForBox(box, color);
            Board::instance()->play(Sound::PegDrop);
			box->setPegState(Peg::State::Visible);
		}//	end if

		if (box->getPegState() != Peg::State::Visible)
			newRowFillState = false;
	} //	end foreach

	//	if (master) code row state changed, go to codeRowFilled
	if (newRowFillState != rowFillState) {
		rowFillState = newRowFillState;
		codeRowFilled(rowFillState);
	}
}

void Game::onPegMouseDoubleClicked(Peg *peg)
{
    foreach(PegBox *box, mCurrentBoxes) {
		if (!box->hasPeg() || box->getPegState() == Peg::State::Invisible) {
			peg->setPos(box->sceneBoundingRect().topLeft());
			break;
		}
	}
}

void Game::changeIndicators()
{
	emit showIndicatorsSignal();
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
	case State::Win:
	case State::Lose:
	case State::Resign:
	case State::WaittingFirstRowFill:
	case State::WaittingHiddenCodeFill:
	case State::WaittingDoneButtonPress:
		return false;
	default:
		return true;
	}
}

void Game::onRevealOnePeg()
{
    if (Rules::instance()->mode() == Mode::HVM && isRunning()) {
        foreach(PegBox *box, mMasterBoxes) {
			if(box->getState() != Box::State::Past) {
                if (box == mMasterBoxes.last()) {
					onResigned();
				} else {
					box->setState(Box::State::Past);
					return;
				}
			}
		}
	}
}

void Game::onResigned()
{
    if (Rules::instance()->mode() == Mode::HVM && isRunning()) {
        mState = State::Resign;
		showMessage();
		freezeScene();
	}
}

void Game::onOkButtonPressed()
{
    if(Rules::instance()->mode() == Mode::MVH) {
		int blacks, whites;
        mPinBoxes.at(mMovesPlayed)->getValue(blacks, whites);
        if(!mSolver->setResponse(blacks, whites)) {
            mMessage->setText(tr("Not Possible, Try Again"));
			return;
		}
        Board::instance()->play(Sound::ButtonPress);
        mOkButton->setVisible(false);

        mPinBoxes.at(mMovesPlayed)->setState(Box::State::Past);

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
        Board::instance()->play(Sound::ButtonPress);
        mState = State::Running;

		unsigned char new_guess[MAX_SLOT_NUMBER];
        for(int i = 0; i < Rules::instance()->pegs(); ++i)
            new_guess[i] = mCurrentBoxes.at(i)->getPegColor();
        Guess::instance()->setGuess(new_guess);

        mPinBoxes.at(mMovesPlayed)->setPins();
        mPinBoxes.at(mMovesPlayed)->setState(Box::State::Past);

        setStateOfList(&mCurrentBoxes, Box::State::Past);
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
    Board::instance()->play(Sound::ButtonPress);
    mState = State::Running;

    mDoneButton->setVisible(false);
    mDoneButton->setEnabled(false);

    setStateOfList(&mCurrentBoxes, Box::State::Past);
    setStateOfList(&mPegBoxes, Box::State::Future);

	unsigned char new_guess[MAX_SLOT_NUMBER];
    for(int i = 0; i < Rules::instance()->pegs(); ++i)
        new_guess[i] = mCurrentBoxes.at(i)->getPegColor();
    Guess::instance()->setCode(new_guess);

    mCurrentBoxes.clear();
	getNextGuess();
	showMessage();
}

void Game::onGuessReady()
{
    mState = State::Running;
	showInformation();

    int box_index = mMovesPlayed*Rules::instance()->pegs();
    for(int i = 0; i < Rules::instance()->pegs(); ++i) {
        createPegForBox(mCodeBoxes.at(box_index + i), Guess::instance()->guess(i));
        mCodeBoxes.at(box_index + i)->setState(Box::State::Past);
	}
    mState = State::WaittingOkButtonPress;
	showMessage();
    mPinBoxes.at(mMovesPlayed)->setState(Box::State::None);
    mOkButton->setEnabled(true);
    mOkButton->setVisible(true);
    mOkButton->setPos(mPinBoxes.at(mMovesPlayed)->pos()-QPoint(0, 39));

    if (Board::instance()->isAutoPutPins())
        mPinBoxes.at(mMovesPlayed)->setPins();
}

void Game::play()
{
	stop();

    if(Rules::instance()->mode() == Mode::MVH)
		playMVH();
    else // (Rules::instance()->m_mode == Mode::HVM)
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
        mSolver = new Solver(this);
        connect(mSolver, SIGNAL(guessDoneSignal()), this, SLOT(onGuessReady()));

	}
    mSolver->interupt();
    mSolver->reset();

    mState = State::WaittingHiddenCodeFill;
	showMessage();
	showInformation();

	//initializing currentrow
    for(int i = 0; i < Rules::instance()->pegs(); ++i) {
        mCurrentBoxes.append(mMasterBoxes.at(i));
        mMasterBoxes.at(i)->setState(Box::State::Current);
	}

	/*	Nothing happening from here till the user fill the master code
	 *	row and press the done button. After the done button is pressed,
	 *	the onm_doneButtonPressed is continuing the game
	 */
}

void Game::playHVM()
{
	qsrand(time(NULL));
    int remaining_colors = Rules::instance()->colors();
	unsigned char hidden_code[MAX_SLOT_NUMBER];
        for(int i = 0; i < Rules::instance()->pegs(); i++) {
			hidden_code[i] = static_cast<unsigned char>(remaining_colors * (qrand()/(RAND_MAX + 1.0)));
            if (!Rules::instance()->sameColors()) {
				for(int j = 0; j < i; j++)
					if (hidden_code[j] <= hidden_code[i])
						hidden_code[i]++;
				remaining_colors--;
			}
            createPegForBox(mMasterBoxes.at(i), hidden_code[i]);
            mMasterBoxes.at(i)->setState(Box::State::None);
		}

    Guess::instance()->setCode(hidden_code);
	setNextRowInAction();
	showMessage();
	showInformation();
    mState = State::WaittingFirstRowFill;
    mOkButton->setPos(mPinBoxes.at(mMovesPlayed)->pos() + QPoint(0, 1));
	// from now on the onPinBoxPressed function continues the game, after the code row is filled
}

void Game::resizeEvent(QResizeEvent *event)
{
	fitInView(sceneRect(), Qt::KeepAspectRatio);
	QGraphicsView::resizeEvent(event);
}

void Game::showInformation()
{
    int possibles = Guess::instance()->possibles();
    qreal weight = Guess::instance()->weight();
    if (Rules::instance()->mode() == Mode::MVH) {
        if (possibles == 1)
		{
            mInformation->setText(tr("The Code Is Cracked!"));
        } else if (possibles > 10000) {
            mInformation->setText(QString("%1    %2: %3").arg(tr("Random Guess")).
                                  arg(tr("Remaining")).arg(Board::instance()->locale().toString(possibles)));
		} else {
            switch (Guess::instance()->algorithm()) {
			case Algorithm::MostParts:
                mInformation->setText(QString("%1: %2    %3: %4").arg(tr("Most Parts")).
                                          arg(Board::instance()->locale().toString(weight)).arg(tr("Remaining")).
                                          arg(Board::instance()->locale().toString(possibles)));
				break;
			case Algorithm::WorstCase:
                mInformation->setText(QString("%1: %2    %3: %4").arg(tr("Worst Case")).
                                          arg(Board::instance()->locale().toString(weight)).arg(tr("Remaining")).
                                          arg(Board::instance()->locale().toString(possibles)));
				break;
			default:
                mInformation->setText(QString("%1: %2    %3: %4").arg(tr("Expected Size")).
                                          arg(Board::instance()->locale().toString(weight)).arg(tr("Remaining")).
                                          arg(Board::instance()->locale().toString(possibles)));
				break;
			}

		}
	} else {
        mInformation->setText(QString("%1: %2   %3: %4   %5: %6").arg(tr("Slots", "", Rules::instance()->pegs())).
                                  arg(Board::instance()->locale().toString(Rules::instance()->pegs())).arg(tr("Colors", "", Rules::instance()->colors())).
                                  arg(Board::instance()->locale().toString(Rules::instance()->colors())).arg(tr("Same Colors")).
                             arg(Rules::instance()->sameColors() ? tr("Yes"): tr("No")));
	}
}

void Game::showMessage()
{
    bool is_MVH = (Rules::instance()->mode() == Mode::MVH);
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
void Game::drawBackground(QPainter *painter, const QRectF &rect)
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
